/* �й���ַ��Ĳ���   V2
   2009/04/20  
   2008/12/08  ����޸�
   2008/06/20  By Liyunming
*/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <depot.h>
#include "utoall.h"
#include "pasdb.h"
#include "ncdef.h"
#include "pasutl.h"
#include "ncurl.h"
#define  NC_MAX_DUMPURL    50000L
static unsigned char  *pNameCacheHash=NULL;       /* �������������Ϣ��Hash��   */
static ncUrlBufHead   *PsUrlBufHead=NULL;
static DEPOT *UrlDp=NULL;       // ���ݿ���
static char  PUrlDbFile[64];    // ��ַ���ļ��ļ���   
static int  iAutoAddUrl=1;      // 1--�Զ����Url��ַ     0--���Զ����
static int  iDomainLevel=2;     // ��������Ĳ��
#define NC_UNKNOW_URL           "../log/ncunknowurl.dat"
#define NC_MAX_UNKNOWURLSIZE    200000000L             /* ����ļ�����  */
#define NC_URL_SOFTVERSION      70502L                 /* Url����汾�� */
#define NC_URL_DBVERSION        90505L                 /* ��ַ��汾    */


char *ncUrlNextDot(char *pUrl);

int  ncUrlSetUrlFlags(int iAuto,int iDomain)
{
    iAutoAddUrl = iAuto;
    iDomainLevel = iDomain;
    return 0;
}



int ncUrlSetUrlBufHead(ncUrlBufHead *psHead)
{
    PsUrlBufHead = psHead;
    return 0;

}

/* װ�����ֻ���
        ��ַ���
������������ID���Ժܿ��ҵ�����
 */
int ncUrlLoadWebClassName()
{
    int iReturn,iCount=0;
    int  lId;
    char caName[32];
    char caVar[32];
    ncUrlWebName s;
    /* ����ַ���ж������Ϣ   */
    while(1) {
        sprintf(caVar,"%03d.webclass.proeim.com",iCount);
        iReturn = ncUrlGetUrlInfo(caVar,caName,&lId);
        if(iReturn == 1) {
            ncUrlAddCacheName(NC_CACHE_WEBCLASS,lId,caName);
            iCount++;
        }
        else {
            break;
        }
    }
#ifdef LDEBUG1
    printf("ncUrlLoadWebClassName %lu Count=%d\n", pNameCacheHash,iCount);
#endif
    return 0;
}

int ncUrlFreeUrlBuf() {
    if(PsUrlBufHead) {
        if(PsUrlBufHead->cShm) {
            utShmHead *psShmHead=utShmCurHead();
            if(psShmHead == NULL) {
                return (-1);
            }
            else {
                utShmFreeArray(psShmHead,NC_LNK_WEBLIST);
            }
        }
        else {
            free(PsUrlBufHead);
            PsUrlBufHead = NULL;
        }
    }
    ncUrlCloseUrlDb();
    return 0;
}





/* ����ַ���ļ�     */
int ncUrlOpenUrlDb(char *pFile)
{
    if(UrlDp) {
        dpclose(UrlDp);
    }
    UrlDp = dpopen(pFile,DP_OREADER,0);
    if(UrlDp == NULL) {
        return (-1);
    }
    else {
        strcpy(PUrlDbFile,pFile);
        return 0;
    }
}

/* �ر���ַ��  */
int ncUrlCloseUrlDb()
{
    if(UrlDp) {
        dpclose(UrlDp);
        UrlDp = NULL;
    }
    return 0;
}


/* ȡ��Url��Ϣ,���ļ���ȡ 
        pDesc --- ��ַ����
        lId   --- ��ַ���
    ����
        0--������
        1--����

*/

int  ncUrlGetUrlInfo(char *pUrl,char *pDesc,int *lId)
{
    ncUrlRecord sRecord;
    char *p;
    int iReturn;
    iReturn = ncUrlGetUrlFromDbFile(pUrl,&sRecord);
    if(iReturn>0) {
        *lId = sRecord.urlValue.nId;
        p = strchr(sRecord.urlValue.caUrl,',');
        if(p) {
            strcpy(pDesc,p+1);
        }
        else {
            strcpy(pDesc,"\0");
        }
        return 1;
    }
    else {
        return 0;
    }
}


int  ncUrlGetUrlInfo0(DEPOT *dp,char *pUrl,char *pDesc,int *lId)
{
    ncUrlRecord sRecord;
    char *p;
    int iReturn;
    iReturn = ncUrlGetUrlFromDbFile0(dp,pUrl,&sRecord);
    if(iReturn>0) {
        *lId = sRecord.urlValue.nId;
        p = strchr(sRecord.urlValue.caUrl,',');
        if(p) {
            strcpy(pDesc,p+1);
        }
        else {
            strcpy(pDesc,"\0");
        }
        return 1;
    }
    else {
        return 0;
    }
}


int  ncUrlPutUrlInfo0(DEPOT *dp,char *pUrl,char *pDesc,int lId)
{
    ncUrlRecord sRecord;
    int iRet;
    utMd5(pUrl,strlen(pUrl),sRecord.urlCode);
    sRecord.urlValue.nId = lId;
    sRecord.urlValue.nLen = sprintf(sRecord.urlValue.caUrl,"%s,%s",pUrl,pDesc)+1;
    sRecord.urlValue.lCount = 1;
    sRecord.urlValue.lLastTime = time(0);
    sRecord.urlValue.nLen = pasTeaEnCode(NC_URL_KEY,sRecord.urlValue.caUrl,sRecord.urlValue.nLen);            
    iRet = dpput(dp,sRecord.urlCode,16,(char *)&sRecord.urlValue,
                                              13+sRecord.urlValue.nLen,DP_DOVER);    
    return iRet;
}


/* ������ַ��Ӧ��WebID
     ���ڴ濪ʼ����

 */
int ncUrlGetUrlClassId(char *pUrl)
{
    int i,k,iReturn;
    ncUrlCode       sKey,*psUrl;
    ncUrlRecord     sRecord;
    char *p;
    
    if(PsUrlBufHead == NULL) {
        return 0;
    }
    p = pUrl;
    for(i=0;i<iDomainLevel&&p;i++) {
        utMd5(p,strlen(p),sKey.urlCode);
        psUrl = (ncUrlCode *)pasLHashLook(PsUrlBufHead->pHash,&sKey);
        if(psUrl) {
            psUrl->lCount++;
            psUrl->lLastTime = time(0);
            return psUrl->nId;
        }
        else {
            p = ncUrlNextDot(p);
        }
    }
    /* ���ڴ���δ�ҵ�,�����ݿ��в���  */
    iReturn = ncUrlGetUrlFromDbFile(pUrl,&sRecord);
    /* �������ڴ���  */
    if(iReturn == 0) {  /* ������    */
        sRecord.urlValue.nId = 0;
        sRecord.urlValue.nLen = strlen(pUrl)+1;
        strcpy(sRecord.urlValue.caUrl,pUrl);
        sRecord.urlValue.lCount = 1;
        sRecord.urlValue.lLastTime = time(0);
        /* �ӵ� unknowurl�ļ���   */
        if(iAutoAddUrl) {
            ncUrlAddUnkonwUrl(pUrl);
        }
    }
    iReturn = pasLHashAdd(PsUrlBufHead->pHash,&sRecord,1);
    if(iReturn < 0) {
        PsUrlBufHead->cFull = 1;
    }
    return sRecord.urlValue.nId;
}


/* ����ַ���ļ��ж���ַ��Ϣ
    ����: <0 ������
          >0 ����url���ݵ��ֽ���
*/
int ncUrlGetUrlFromDbFile(char *pUrl,ncUrlRecord *psRecord)
{
    if(UrlDp == NULL) { /* �����ݿ�δ�򿪣����Զ���  */
        UrlDp = dpopen(PUrlDbFile,DP_OREADER,0);
        if(UrlDp == NULL) {
            return (-1);
        }
    }
    return ncUrlGetUrlFromDbFile0(UrlDp,pUrl,psRecord);
}



int ncUrlGetUrlFromDbFile0(DEPOT *dp,char *pUrl,ncUrlRecord *psRecord)
{
    unsigned char   *p;
    int             i,iReturn;
    p = pUrl;
    for(i=0;i<iDomainLevel&&p;i++) {
        utMd5(p,strlen(p),psRecord->urlCode);
        iReturn = dpgetwb(dp,psRecord->urlCode,16,0,sizeof(ncUrlValue),(char *)&psRecord->urlValue);
        if(iReturn > 0) {
            psRecord->urlValue.nLen = pasTeaDeCode(NC_URL_KEY,psRecord->urlValue.caUrl,psRecord->urlValue.nLen);            
            return iReturn;
        }
        else {
            p = ncUrlNextDot(p);
        }
    }
    return 0;
}




/* ����һ���µ�Url��

 */
int ncUrlGenUrlDb(char *pDbFile,int iMaxUrl)
{
    ncUrlRecord sRecord;
    int  iReturn;
    char caUrl[64];
    char caName[64];
    DEPOT *dp;
    dp = dpopen(pDbFile, DP_OWRITER | DP_OCREAT | DP_OTRUNC,iMaxUrl);
    if(dp == NULL) {
        printf(" DataBase Create Error %s \n",pDbFile);
    }
    memset(&sRecord,0,sizeof(ncUrlRecord));
    ncUrlUpdateVersion(dp,NC_URL_SOFTVERSION,NC_URL_DBVERSION,time(0));
    dpclose(dp);
    return 1;
}


/* ���°汾��Ϣ�����ݿ���   */
int ncUrlUpdateVersion(DEPOT *dp,unsigned long lVersion,unsigned long lDbVersion,unsigned long lUpdate)
{
    ncUrlRecord sRecord;
    char caUrl[64];
    char caName[64];
    int  iReturn;
    memset(&sRecord,0,sizeof(ncUrlRecord));
    strcpy(caUrl,"ProeimUrlv7.pronetway.com");
    sprintf(caName,"%lu,%lu,%lu",lVersion,lDbVersion,lUpdate);
    utMd5(caUrl,strlen(caUrl),sRecord.urlCode);
    sRecord.urlValue.nId = 6009;
    sRecord.urlValue.nLen = sprintf(sRecord.urlValue.caUrl,"%s,%s",caUrl,caName)+1;
    sRecord.urlValue.lCount = 0;
    sRecord.urlValue.lLastTime = time(0);
    sRecord.urlValue.nLen = pasTeaEnCode(NC_URL_KEY,sRecord.urlValue.caUrl,sRecord.urlValue.nLen);            
    iReturn = dpput(dp,sRecord.urlCode,16,(char *)&sRecord.urlValue,
                                                  13+sRecord.urlValue.nLen,DP_DOVER);
    return iReturn;
}


/* ȡ�ð汾��Ϣ   */
int ncUrlGetUrlVersion(char *pEngVer,char *pDbVer,char *pUpdate,int *iSumRecord)
{
    ncUrlRecord sRecord;
    char caUrl[64];
    unsigned long lV1,lV2,lT;
    int iReturn,n;
    memset(&sRecord,0,sizeof(ncUrlRecord));
    strcpy(caUrl,"ProeimUrlv7.pronetway.com");
    iReturn = ncUrlGetUrlFromDbFile(caUrl,&sRecord);
    if(iReturn > 0) {
        n = utStrGetSomeWordBy(sRecord.urlValue.caUrl,",",4,
                             UT_TYPE_STRING,63,caUrl,
                             UT_TYPE_ULONG,4,&lV1,
                             UT_TYPE_ULONG,4,&lV2,
                             UT_TYPE_ULONG,4,&lT);
        sprintf(pEngVer,"V%d.%02d.%02d",lV1/10000,(lV1/100)%100,lV1%100);
        sprintf(pDbVer,"V%d.%02d.%02d",lV2/10000,(lV2/100)%100,lV2%100);
        sprintf(pUpdate,"%s",utTimFormat("%Y-%m-%d %H:%M:%S",lT));
        *iSumRecord = dprnum(UrlDp) + 19800000L;
        return 1;
    }
    else {
        return 0;
    }
}









/* ������ַ���������   
      ����   0--������
             sid--��ַ����ַ���е�Id
*/

int ncUrlIsInUrlClass(char *pUrl,long lId)
{
    long iReturn;
    iReturn = ncUrlGetUrlClassId(pUrl);
    if(iReturn == lId) {
        return 1;
    }
    else {
        return 0;
    }
}


/*   ������ַ���ID
 */
int ncUrlGetClassIdByName(char *pName)
{
    int iReturn;
    iReturn =  ncUrlGetChcheIdByName(NC_CACHE_WEBCLASS,pName);
    return iReturn;
}



/* Url�����ļ��ĸ�ʽ    */
/*
# Create at 2008/06/26  By Pronetway
# Date ClassId  Url   Descript
#
*/
int ncUrlAddUnkonwUrl(char *pUrl)
{
    FILE *fp;
    unsigned long lTime;
    long lSize;
    if(utStrIsDoMain(pUrl)) {
        lTime = time(0);
        lSize = utFileSize(NC_UNKNOW_URL);
        if(lSize > 200000000L) {
            char caFile[128];
            sprintf(caFile,"%s_1",NC_UNKNOW_URL);
            remove(caFile);
            rename(NC_UNKNOW_URL,caFile);
        }
        fp = fopen(NC_UNKNOW_URL,"a");
        if(fp) {
            fprintf(fp,"%s\n",pUrl);
            fclose(fp);
        }
    }
    return 0;
}

/* ��Url,��������뵽��ַ����  
 */
int ncUrlCvtText2UrlDb(char *pFile,char *pDbFile)
{
    FILE *fp,*fpo;
    DEPOT    *dp;
    char caFile[128];
    ncUrlRecord   sRecord;
    char            *p,buf[256];
    char            caUrl[64],caName[64];
    unsigned long   lCount;
    unsigned long   lId=0;
    int iCount,iReturn,lSize,n;
    dp = dpopen(pDbFile, DP_OWRITER,0);
    if(dp == NULL) {
        return (-1);
    }
    sprintf(caFile,"%s.log");
    lSize = utFileSize(caFile);
    if(lSize > NC_MAX_UNKNOWURLSIZE ) { /* ��־�ļ�����ָ���ĳ���  */
        char caFile1[128];
        sprintf(caFile1,"%s_1",NC_UNKNOW_URL );
        remove(caFile1);
        rename(caFile,caFile1);
    }
    fp = fopen(pFile,"r");
    if(fp) {
        fpo = fopen(caFile,"a");
        p = fgets(buf,255,fp);
        iCount = 0;
        while(p) {
            p = utStrSkipSpaces(buf);
            if(*p != '#' && !utStrIsSpaces(p)) {
                strcpy(caUrl,"\0");
                strcpy(caName,"\0");
                n = utStrGetSomeWordBySp(p,2,
                                     UT_TYPE_STRING,64,caUrl,
                                     UT_TYPE_STRING,64,caName);
                if(strcasecmp(caUrl,"ClassName")==0) {
                    lId = ncUrlGetClassIdByName(caName);
                    if(lId > 0) {
                        if(fpo) {
                            fprintf(fpo,"ClassName  %s\n",caName);
                        }
                    }
                }
                else {
                    if(utStrIsDoMain(caUrl)) {
                        utMd5(caUrl,strlen(caUrl),sRecord.urlCode);
                        sRecord.urlValue.nId = 0;
                        sRecord.urlValue.nLen = sprintf(sRecord.urlValue.caUrl,"%s,%s",caUrl,caName)+1;
                        sRecord.urlValue.lCount = 0;
                        sRecord.urlValue.lLastTime = time(0);
                        sRecord.urlValue.nLen = pasTeaEnCode(NC_URL_KEY,sRecord.urlValue.caUrl,sRecord.urlValue.nLen);            
                        iReturn = dpput(dp,sRecord.urlCode,16,(char *)&sRecord.urlValue,
                                              13+sRecord.urlValue.nLen,DP_DOVER);
                        iCount++;
                        if(fpo) {
                            fprintf(fpo,"-31%s     \"%s\"\n",caUrl,caName);
                        }
                    }
                }
            }
        }
        p = fgets(buf,255,fp);
    }
    fclose(fp);
    remove(pFile);
    if(fpo) {
        fclose(fpo);
    }
    dpclose(dp);
    pasLog(PAS_LOG_EVENT,"Url %lu Records Load",iCount);
    return iCount;
}


/* �����ݿ��н���ַ�⵼����  */
int ncUrlDumpUrlFromDb(char *pDbFile,char *pFile)
{
    DEPOT *dp;
    FILE *fpo;
    char *p;
    ncUrlRecord sRecord;
    int iReturn;
    dp = dpopen(pDbFile,DP_OREADER,0);
    if(dp == NULL) {
        printf("DbFile not open %s\n",pDbFile);
        return (-1);
    }
    fpo = fopen(pFile,"w");
    if(fpo == NULL) {
        dpclose(dp);
        printf(" File Open Error %s \n",pFile);
        return (-1);
    }
    iReturn = dpiterinit(dp);
    p = dpiternext(dp,&iReturn);
    while(p) {
        memset(&sRecord,0,sizeof(sRecord));
        iReturn = dpgetwb(dp,p,16,0,sizeof(ncUrlValue),(char *)&sRecord.urlValue);
        if(iReturn > 0) {
//            sRecord.urlValue.nLen = utDesDecrypt(sRecord.urlValue.caUrl,sRecord.urlValue.nLen,
//                          sRecord.urlValue.caUrl,NC_URL_KEY);
            sRecord.urlValue.nLen = pasTeaDeCode(NC_URL_KEY,sRecord.urlValue.caUrl,sRecord.urlValue.nLen);            
            fprintf(fpo,"%04d  %s \n",sRecord.urlValue.nId,sRecord.urlValue.caUrl);
        }
        free(p);
        p = dpiternext(dp,&iReturn);
    }
    fclose(fpo);
    dpclose(dp);
    return 0;
}

/* ��ʼ��Name Cache */

int ncUrlInitNameCache()
{
    if(pNameCacheHash) {
        free(pNameCacheHash);
    }
printf("Init Name Cache, Pid is %d NameCache==%lu\n",getpid(),pNameCacheHash);    
    pNameCacheHash = (unsigned char *)pasLHashInit(NC_MAX_NAMECACHE,NC_MAX_NAMECACHE,
                        sizeof(ncUrlWebName),0,5);
    if(pNameCacheHash==NULL) {
        pasLog(PAS_LOG_ERROR,"Name Cache Init Error ");
        return (-1);
    }                   
    return 0;
}





int ncUrlReadAllWebClass(DEPOT *dp,ncUrlWebName *ps,int iMax)
{
    int iReturn,iCount=0;
    int  lId;
    char caName[32];
    char caVar[32];
    while(iCount < iMax) {
        sprintf(caVar,"%03d.webclass.proeim.com",iCount);
        iReturn = ncUrlGetUrlInfo0(dp,caVar,caName,&lId);
        if(iReturn == 1) {
            ps[iCount].lId = lId;
            strcpy(ps[iCount].name,caName);
            iCount++;
        }
        else {
            break;
        }
    }
    return iCount;
}

int ncUrlSaveAllWebClass(DEPOT *dp,ncUrlWebName *ps,int iSum)
{
    int iReturn,iCount=0;
    ncUrlRecord sRecord;
    int  lId;
    char caName[32];
    char caVar[32];
    for(iCount=0;iCount<iSum;iCount++) {
        sprintf(caVar,"%03d.webclass.proeim.com",iCount);
        utMd5(caVar,strlen(caVar),sRecord.urlCode);
        sRecord.urlValue.nId = ps[iCount].lId;
        sRecord.urlValue.nLen = sprintf(sRecord.urlValue.caUrl,"%s,%s",caVar,ps[iCount].name)+1;
        sRecord.urlValue.lCount = 1;
        sRecord.urlValue.lLastTime = time(0);
        sRecord.urlValue.nLen = pasTeaEnCode(NC_URL_KEY,sRecord.urlValue.caUrl,sRecord.urlValue.nLen);            
        iReturn = dpput(dp,sRecord.urlCode,16,(char *)&sRecord.urlValue,
                                              13+sRecord.urlValue.nLen,DP_DOVER);
    }
    return 0;
}



int  ncUrlAddCacheName(int flags,unsigned long lId,char *pName)
{
    ncUrlWebName s;
    memset(&s,0,sizeof(s));
    s.lId = lId;
    s.flags = flags;
    utStrncpy(s.name,pName,30);
    return pasLHashAdd(pNameCacheHash,&s,1);
}

/* ����IDȡ����  
      Flags --- ���
        NC_CACHE_WEBCLASS ---��ַ�����
      
 */
char *ncUrlGetCacheName(int flags,unsigned long lId,char *pDefault)
{
    if(pNameCacheHash) {
        ncUrlWebName s,*ps;
        s.lId = lId;
        s.flags = flags;
        ps = (ncUrlWebName *)pasLHashLook(pNameCacheHash,&s);

        if(ps) {
#ifdef LDEBUG1
           printf("ncUrlGetClassNameById %lu  Id=%lu Name=[%s] \n",
                 pNameCacheHash,lId,ps->name);
#endif
            
            return ps->name;
        }
        else {
            return pDefault;
        }
    }
    else {
        return pDefault;
    }
}


/*  ��������ȡID      
      ʹ��ȫ�������ķ���������Ч�ʲ���̫��
 */
int ncUrlGetChcheIdByName(int flags,char *pName)
{

    if(pNameCacheHash) {
        pasLHashInfo    sHashInfo;
        ncUrlWebName    *ps;
        ps = (ncUrlWebName *)pasLHashFirst(pNameCacheHash,&sHashInfo);
        while(ps) {
            if(ps->flags == flags && strcasecmp(ps->name,pName)==0) {
                return ps->lId;
            }
           ps = (ncUrlWebName *)pasLHashNextS(&sHashInfo);
        }
    }
    return 0;
}


/* ����IDȡ��ַ�������  */
char *ncUrlGetClassNameById(unsigned long lId,char *pDefault)
{
    
    return ncUrlGetCacheName(NC_CACHE_WEBCLASS,lId,pDefault);
}


int ncUrlSetUrlUpdate(utShmHead *psShmHead)
{
    ncUrlBufHead *psHead;
    psHead = (ncUrlBufHead *)utShmArray(psShmHead,NC_LNK_WEBLIST);
    if(psHead == NULL) {
        printf(" WebList Buffer not ready \n");
        return (-1);
    }
    else {
        psHead->cUpMark = 1;
    }
    return 0;
}

    



/* �޸���ַ����Ϣ 
      �޸���ַ��Ϣ
 */
int ncUrlUpdateUrl(char *pUrl,int lId,char *pDesc)
{
    ncUrlCode       sKey,*psUrl;
    ncUrlRecord     sRecord;
    int             iReturn;
    DEPOT           *dp;
    /* �޸��ڴ��������Ϣ  */
    
    utMd5(pUrl,strlen(pUrl),sKey.urlCode);
    psUrl = (ncUrlCode *)pasLHashLook(PsUrlBufHead->pHash,&sKey);
    if(psUrl) {
        psUrl->lCount++;
        psUrl->lLastTime = time(0);
        psUrl->nId = lId;
    }


    dp = dpopen(PUrlDbFile, DP_OWRITER,0);
    if(dp == NULL) {
        pasLog(PAS_LOG_ERROR,"DataBase File %s Open Error \n",PUrlDbFile);
        return (-1);
    }
    utMd5(pUrl,strlen(pUrl),sRecord.urlCode);
    sRecord.urlValue.nId = lId;
    sRecord.urlValue.cFlags = 9;
    sRecord.urlValue.nLen = sprintf(sRecord.urlValue.caUrl,"%s,%s",pUrl,pDesc)+1;
    sRecord.urlValue.lLastTime = time(0);
    sRecord.urlValue.nLen = pasTeaEnCode(NC_URL_KEY,sRecord.urlValue.caUrl,sRecord.urlValue.nLen);            
    iReturn = dpput(dp,sRecord.urlCode,16,(char *)&sRecord.urlValue,
                                              13+sRecord.urlValue.nLen,DP_DOVER);
    dpclose(dp);
    if(iReturn == 1) {
        return 0;
    }
    else {
        return (-1);
    }
}

/* ɾ����ַ����Ϣ 
      �޸���ַ��Ϣ
 */
int ncUrlDeleteUrl(char *pUrl)
{
    ncUrlCode       sKey,*psUrl;
    ncUrlRecord     sRecord;
    int             iReturn;
    DEPOT           *dp;
    /* �޸��ڴ��������Ϣ  */
    
    utMd5(pUrl,strlen(pUrl),sKey.urlCode);
    pasLHashDel(PsUrlBufHead->pHash,&sKey);
    dp = dpopen(PUrlDbFile, DP_OWRITER,0);
    if(dp == NULL) {
        pasLog(PAS_LOG_ERROR,"DataBase File %s Open Error \n",PUrlDbFile);
        return (-1);
    }
    utMd5(pUrl,strlen(pUrl),sRecord.urlCode);
    iReturn = dpout(dp,sRecord.urlCode,16);
    dpclose(dp);
    if(iReturn == 1) {
        return 0;
    }
    else {
        return (-1);
    }
}


/* ��ַ������ļ�   
     ���ļ���׷��һ����¼
     
     ����:
         0--�ɹ�
         (-1)--ʧ��
 */
int  ncUrlPutUpdateUrl(FILE *fp,int iClass,char *pUrl,char *pDesc)
{
    ncUrlUpRecord sRecord;
    int n;
    memset(&sRecord,0,sizeof(ncUrlUpRecord));
    sRecord.nId = NC_URL_UPFILE_ID;
    sRecord.nType = NC_URL_UPFILE_URL;
    sRecord.cOpt = 'a';
    sRecord.nClass = iClass;
    n = snprintf(sRecord.buf,127,"%s,%s",pUrl,pDesc) + 1;
    sRecord.cLen = pasTeaEnCode(NC_URL_KEY,sRecord.buf,n);
    sRecord.lTime = time(0);
    n = fwrite(&sRecord,1,sizeof(ncUrlUpRecord),fp);
    if(n!=sizeof(ncUrlUpRecord)) {
        return (-1);
    }
    else {
        return 0;
    }
}


/* ��ַ������ļ�   
     ���ļ���׷��һ�����
     
     ����:
         0--�ɹ�
         (-1)--ʧ��
 */
int  ncUrlPutUpdateClass(FILE *fp,int iClass,char *ClassName)
{
    ncUrlUpRecord sRecord;
    int n;
    memset(&sRecord,0,sizeof(ncUrlUpRecord));
    sRecord.nId = NC_URL_UPFILE_ID;
    sRecord.nType = NC_URL_UPFILE_CLASS;
    sRecord.cOpt = 'a';
    sRecord.nClass = iClass;
    n = snprintf(sRecord.buf,127,"%s",ClassName) + 1;
    sRecord.cLen = pasTeaEnCode(NC_URL_KEY,sRecord.buf,n);
    sRecord.lTime = time(0);
    n = fwrite(&sRecord,1,sizeof(ncUrlUpRecord),fp);
    if(n!=sizeof(ncUrlUpRecord)) {
        return (-1);
    }
    else {
        return 0;
    }
}


/* ��ַ������ļ�   
     ���ļ���׷��һ�����
     
     ����:
         0--�ɹ�
         (-1)--ʧ��
 */
int  ncUrlDelUpdateUrl(FILE *fp,char *pUrl)
{
    ncUrlUpRecord sRecord;
    int n;
    memset(&sRecord,0,sizeof(ncUrlUpRecord));
    sRecord.nId = NC_URL_UPFILE_ID;
    sRecord.nType = NC_URL_UPFILE_URL;
    sRecord.cOpt = 'd';
    sRecord.nClass = 0;
    n = snprintf(sRecord.buf,127,"%s",pUrl) + 1;
    sRecord.cLen = pasTeaEnCode(NC_URL_KEY,sRecord.buf,n);
    sRecord.lTime = time(0);
    n = fwrite(&sRecord,1,sizeof(ncUrlUpRecord),fp);
    if(n!=sizeof(ncUrlUpRecord)) {
        return (-1);
    }
    else {
        return 0;
    }
}


/* ��ַ������ļ�   
     ���ļ���׷��һ�����
     ����:
         0--�ɹ�
         (-1)--ʧ��
 */
int  ncUrlDelUpdateClass(FILE *fp,char *pClass)
{
    ncUrlUpRecord sRecord;
    int n;
    memset(&sRecord,0,sizeof(ncUrlUpRecord));
    sRecord.nId = NC_URL_UPFILE_ID;
    sRecord.nType = NC_URL_UPFILE_CLASS;
    sRecord.cOpt = 'd';
    sRecord.nClass = 0;
    n = snprintf(sRecord.buf,127,"%s",pClass) + 1;
    sRecord.cLen = pasTeaEnCode(NC_URL_KEY,sRecord.buf,n);
    sRecord.lTime = time(0);
    n = fwrite(&sRecord,1,sizeof(ncUrlUpRecord),fp);
    if(n!=sizeof(ncUrlUpRecord)) {
        return (-1);
    }
    else {
        return 0;
    }
}




/* ��ַ������ļ�   
     ���ļ��ж���һ����¼
     iOpt  -- ���� a--����  d-ɾ��
     iType -- 11760 Url    11761 Class
     iClass-- ���
     pUrl  -- ��ַ
     pDesc -- ����

   ����
      0--����
      1--����
 */
int  ncUrlGetUpdateUrl(FILE *fp,int *iOpt,int *iType,int *iClass,char *pUrl,char *pDesc)
{
    ncUrlUpRecord sRecord;
    unsigned char *p;
    int n;
    memset(&sRecord,0,sizeof(ncUrlUpRecord));
    p = &sRecord;
    
    while(1) {
        n = fread(p,1,4,fp);
        if(n!=4) {
            return (-1);
        }
        else {
            if(sRecord.nId == NC_URL_UPFILE_ID && 
                    (sRecord.nType == NC_URL_UPFILE_URL || sRecord.nType == NC_URL_UPFILE_CLASS)) {
                n = fread(p+4,1,8,fp);
                if(n==8) {
                    n = fread(p+12,1,sRecord.cLen,fp);
                    if(n==sRecord.cLen) {
                        sRecord.cLen =  pasTeaDeCode(NC_URL_KEY,sRecord.buf,n);  
                        p = strchr(sRecord.buf,',');
                        if(p) {
                            *p = 0;
                            strcpy(pUrl,sRecord.buf);
                            strcpy(pDesc,p+1);
                        }
                        else {
                            strcpy(pUrl,sRecord.buf);
                            strcpy(pDesc,"\0");
                        }
                        *iClass = sRecord.nClass;
                        *iOpt   = sRecord.cOpt;
                        *iType  = sRecord.nType;
                        return 1;
                    }
                    else {
                        return (-1);
                    }
                }
            }
        }
    }
    return (-1);
}



/* ���ı���ʽ���ļ�ת��Ϊ�����ļ���ʽ    

 */                
int ncUrlCvtUpFile2Txt(char *pUpFile,char *pTxtFile)
{
    FILE *fp;
    FILE *fpo;
    int iOpt,iType,iClass,iCount=0,iReturn;
    char caUrl[128],caDesc[128];
    fp = fopen(pUpFile,"r");
    if(fp == NULL) {
        printf("File %s open error\n",pTxtFile);
        return (-1);
    }
    fpo = fopen(pTxtFile,"w");
    if(fpo == NULL) {
        fclose(fp);
        printf("File %s Open Error \n",pUpFile);
        return (-1);
    }
    iReturn = ncUrlGetUpdateUrl(fp,&iOpt,&iType,&iClass,caUrl,caDesc);
    while(iReturn==1) {
        iCount++;
        fprintf(fpo,"[%d] %d %5lu %4lu %-31s %s\n",
            iCount,iOpt,iType,iClass,caUrl,caDesc);
        iReturn = ncUrlGetUpdateUrl(fp,&iOpt,&iType,&iClass,caUrl,caDesc);
    }
    fclose(fp);
    fclose(fpo);
    return 0;
}

/*  
    �������ļ���Udb�Ƚ��Ժ���뵽����
        ����¼�Ѵ��ڣ��Ƚ�flags, flags >= ԭ����flags�����
 */
int ncUrlCvtUpFile2Udb(char *pUpFile,char *pDbFile,int iDebug,int iFlags,char *pMsg)
{
    
    FILE *fp;
    FILE *fpo;
    ncUrlRecord sRecord;
    char *p1;
    DEPOT *dp;
    int iOpt,iType,iClass,iCount=0,iReturn,iRet;
    int iSumClass;
    int iBackLevel=iDomainLevel;
    int iModiClass=0,i,j;
    int iSumAddUrl=0,iSumUpUrl=0,iSumDelUrl=0,iSumAddClass=0,iSumDelClass=0,iSumUpClass=0;
    long lId1;
    char caUrl[128],caDesc[128],caDesc1[128];
    ncUrlWebName   sClass[200];
    strcpy(pMsg,"\0");
    
    iDomainLevel = 1;
    
    fp = fopen(pUpFile,"r");
    if(fp == NULL) {
        printf("File %s open error\n",pUpFile);
        return (-1);
    }
    dp = dpopen(pDbFile, DP_OWRITER,0);
    if(dp == 0) {
        printf("Udb File Open Error %s \n",pDbFile);
        return (-1);
    }
    iSumClass = ncUrlReadAllWebClass(dp,sClass,200);
    if(iDebug) {
        fpo = fopen("urlerror.log","w");
        if(fpo == NULL) {
            fclose(fp);
            printf("File %s Open Error \n",pUpFile);
            return (-1);
        }
    }
    iReturn = ncUrlGetUpdateUrl(fp,&iOpt,&iType,&iClass,caUrl,caDesc);
    while(iReturn==1) {
        if(strstr(caUrl,"updatelog.proeim.com")==NULL) {
            iCount++;
            if(iOpt == 'a') { /* ���Ӽ�¼  */
                if(iType == NC_URL_UPFILE_URL) {
                    iRet = ncUrlGetUrlFromDbFile0(dp,caUrl,&sRecord);
//                    iRet =  ncUrlGetUrlInfo0(dp,caUrl,caDesc1,&lId1);
                    if(iRet > 0) { /* �Ѵ���  */
                        if(iFlags >= sRecord.urlValue.cFlags) { 
                            iSumUpUrl++;
                            utMd5(caUrl,strlen(caUrl),sRecord.urlCode);
                            sRecord.urlValue.nId = iClass;
                            sRecord.urlValue.nLen = sprintf(sRecord.urlValue.caUrl,"%s,%s",caUrl,caDesc)+1;
                            sRecord.urlValue.lCount = 1;
                            sRecord.urlValue.cFlags = iFlags;
                            sRecord.urlValue.lLastTime = time(0);
                            sRecord.urlValue.nLen = pasTeaEnCode(NC_URL_KEY,sRecord.urlValue.caUrl,sRecord.urlValue.nLen);            
                            iRet = dpput(dp,sRecord.urlCode,16,(char *)&sRecord.urlValue,
                                              13+sRecord.urlValue.nLen,DP_DOVER);
                            if(iDebug) {
                                fprintf(fpo,"[Update]%-31s %02d[%s] ==>%02d[%s] %d\n",caUrl, lId1,caDesc1,
                                         iClass,caDesc,iRet);
                            }
                        }
                    }
                    else {
                        utMd5(caUrl,strlen(caUrl),sRecord.urlCode);
                        sRecord.urlValue.nId = iClass;
                        sRecord.urlValue.nLen = sprintf(sRecord.urlValue.caUrl,"%s,%s",caUrl,caDesc)+1;
                        sRecord.urlValue.lCount = 1;
                        sRecord.urlValue.cFlags = iFlags;
                        sRecord.urlValue.lLastTime = time(0);
                        sRecord.urlValue.nLen = pasTeaEnCode(NC_URL_KEY,sRecord.urlValue.caUrl,sRecord.urlValue.nLen);            
                        iRet = dpput(dp,sRecord.urlCode,16,(char *)&sRecord.urlValue,
                                              13+sRecord.urlValue.nLen,DP_DOVER);
                        if(iDebug) {
                            fprintf(fpo,"[AddUrl]%-31s %02d[%s] %d\n", caUrl,iClass,caDesc,iRet);
                        }
                        iSumAddUrl++;
                    }
                }
                else if(iType == NC_URL_UPFILE_CLASS) {
                    for(i=0;i<iSumClass;i++) {
                        if(sClass[i].lId == iClass) {
                            if(iDebug) {
                                fprintf(fpo,"[UpClass]%-31s %04d ==>%-31s %04d\n",
                                   sClass[i].name,sClass[i].lId,
                                   caUrl,iClass);
                            }
                            strcpy(sClass[i].name,caUrl);
                            iSumUpClass++;
                            iModiClass = 1;
                            break;
                        }
                    }
                    if(i==iSumClass) {
                        if(iDebug) {
                            fprintf(fpo,"[AddClass]%-31s %04d\n",
                                   caUrl,iClass);
                        }
                        strcpy(sClass[i].name,caUrl);
                        sClass[i].lId = iClass;
                        iSumAddClass++;
                        iSumClass++;
                        iModiClass = 1;
                    }
                }
            }
            else if(iOpt == 'd') {
                if(iType == NC_URL_UPFILE_URL) {
                    utMd5(caUrl,strlen(caUrl),sRecord.urlCode);
                    iRet = dpout(dp,sRecord.urlCode,16);
                    if(iDebug) {
                        fprintf(fpo,"[DelUrl]%s  %d\n",caUrl,iRet);
                    }
                    iSumDelUrl++;
                
                }
            }
        }
        iReturn = ncUrlGetUpdateUrl(fp,&iOpt,&iType,&iClass,caUrl,caDesc);
    }
    if(iModiClass) {
        ncUrlSaveAllWebClass(dp,sClass,iSumClass);
    }
    fclose(fp);
    if(iSumAddUrl > 0) {
        utStrAddF(pMsg,"������ַ%lu��;",iSumAddUrl);
    }
    if(iSumUpUrl > 0) {
        utStrAddF(pMsg,"������ַ%lu��;",iSumUpUrl);
    }
    if(iSumDelUrl > 0) {
        utStrAddF(pMsg,"ɾ����ַ%lu��;",iSumDelUrl);
    }
    if(iSumAddClass > 0) {
        utStrAddF(pMsg,"������� %lu��;",iSumAddClass);
    }
    if(iSumAddClass > 0) {
        utStrAddF(pMsg,"�������%lu��;",iSumUpClass);
    }
    iDomainLevel =  iBackLevel;
    if(!utStrIsSpaces(pMsg)) {
        char caMsg[128];
        sprintf(caMsg,"[%s] %s",utTimFormat("%Y-%m-%d %H:%M:%S",time(0)),pMsg);
        ncUrlSaveLog(dp,caMsg);
        ncUrlUpdateVersion(dp,NC_URL_SOFTVERSION,NC_URL_DBVERSION,time(0));
    }
    dpclose(dp);
    return 0;
}

/*   ����ַ���ݴ�UDB�е�����,����һ���ı��ļ�
     �ļ���ʽ:
        [ID]  Url  Name
 */
int ncUrlDumpUdb2Txt(char *pDbFile,char *pTxtFile,unsigned long lTime)
{
    FILE *fpo;
    DEPOT *dp;
    ncUrlValue sValue;
    int  iClass,iReturn;
    char caUrl[128],caDesc[128];
    char *p;
    int iCount=0;
    fpo = fopen(pTxtFile,"w");
    if(fpo == NULL) {
        printf("File %s open error\n",pTxtFile);
        return (-1);
    }
    dp = dpopen(pDbFile, DP_OREADER  ,0);
    if(dp == 0) {
        printf("Udb File Open Error %s \n",pDbFile);
        return (-1);
    }
    fprintf(fpo,"# URL  DumpTime:%s \n",utTimFormat("%Y-%m-%d %H:%M:%S",time(0)));
    fprintf(fpo,"# Dump From %s  Date >= %s \n",
          pDbFile,utTimFormat("%Y-%m-%d %H:%M:%S",lTime));
    iReturn = dpiterinit(dp);
    p = dpiternext(dp,NULL);
    while(p) {
        iReturn = dpgetwb(dp,p,16,0,sizeof(ncUrlValue),(char *)&sValue);
        free(p);
        sValue.nLen = pasTeaDeCode(NC_URL_KEY,sValue.caUrl,sValue.nLen);    
        if(sValue.lLastTime >= lTime) {        
            p = strchr(sValue.caUrl,',');
            if(p) {
                *p=0;
                strcpy(caUrl,sValue.caUrl);
                strcpy(caDesc,p+1);
            }
            else {
                strcpy(caUrl,sValue.caUrl);
                strcpy(caDesc,"\0");
            }
            if(strstr(caUrl,"webclass.proeim.com")) {  /* ��ַ���  */
                fprintf(fpo," ClassName %-16s   [%04d]\n",caDesc,sValue.nId);
            }
            else if(strcmp(caUrl,"ProeimUrlv7.pronetway.com")==0) {
                int n;
                unsigned long lV1,lV2,lT;
                n = utStrGetSomeWordBy(caDesc,",",3,
                             UT_TYPE_ULONG,4,&lV1,
                             UT_TYPE_ULONG,4,&lV2,
                             UT_TYPE_ULONG,4,&lT);
                fprintf(fpo,"# Udb EngVersion V%d.%02d.%02d",lV1/10000,(lV1/100)%100,lV1%100);
                fprintf(fpo," UrlDbVersion V%d.%02d.%02d",lV2/10000,(lV2/100)%100,lV2%100);
                fprintf(fpo," LastUpdate:%s\n",utTimFormat("%Y-%m-%d %H:%M:%S",lT));
                fprintf(fpo,"##############################################################################\n");
            }
            else {
                fprintf(fpo," [%04d] %-31s  %-63s  %s\n",
                    sValue.nId,caUrl,ncAnyCvtQuotaWord(caDesc),utTimFormat("%Y-%m-%d %H:%M:%S",sValue.lLastTime));
            } 
            iCount++;
        }
        p = dpiternext(dp,NULL);
    }
    fprintf(fpo,"# End Dump, SumRecord is %d\n",iCount);
    fclose(fpo);
    dpclose(dp);
    return iCount;
}

/*   ����ַ���ݴ�UDB�е�����,����һ�������ļ�
     ע���� iSetFlags != (-1), ��ÿ�ε����ļ�¼������50000��
 */
int ncUrlDumpUdb2UpFile(char *pDbFile,
                        char *pTxtFile,
                        unsigned long lStime,
                        unsigned long lEtime,
                        unsigned long lMaxRecord,
                        int           iFlags,
                        int           iSetFlags)
{
    FILE *fpo;
    DEPOT *dp;
    ncUrlValue sValue;
    int  iClass,iReturn,i;
    char caUrl[128],caDesc[128];
    char *p,*p1;
    char **pKey;
    int iCount=0;
    fpo = fopen(pTxtFile,"w");
    if(fpo == NULL) {
        printf("File %s open error\n",pTxtFile);
        return (-1);
    }
    if(iSetFlags == (-1)) {
        dp = dpopen(pDbFile, DP_OREADER  ,0);
    }
    else {
        dp = dpopen(pDbFile, DP_OWRITER  ,0);
    }
    if(dp == 0) {
        printf("Udb File Open Error %s \n",pDbFile);
        return (-1);
    }
    if(iFlags != (-1)) {
        if(lMaxRecord == 0 || lMaxRecord > NC_MAX_DUMPURL) {
            lMaxRecord = NC_MAX_DUMPURL;
        }
        pKey = (char **)malloc(lMaxRecord* sizeof(char *));
        if(pKey == NULL) {
            dpclose(dp);
            fclose(fpo);
            pasLog(PAS_LOG_ERROR,"Memory alloc Error");
            return (-1);
        }
        memset(pKey,0,lMaxRecord* sizeof(char *));
    }
    iReturn = dpiterinit(dp);
    p = dpiternext(dp,NULL);
    while(p) {
        iReturn = dpgetwb(dp,p,16,0,sizeof(ncUrlValue),(char *)&sValue);
        sValue.nLen = pasTeaDeCode(NC_URL_KEY,sValue.caUrl,sValue.nLen);    
        if(sValue.lLastTime >= lStime && (sValue.lLastTime <= lEtime ||lEtime == 0) 
                && (iFlags == (-1) || sValue.cFlags == iFlags)
                && (lMaxRecord == 0 || iCount < lMaxRecord)) { 
            p1 = strchr(sValue.caUrl,',');
            if(p1) {
                *p1=0;
                strcpy(caUrl,sValue.caUrl);
                strcpy(caDesc,p1+1);
            }
            else {
                strcpy(caUrl,sValue.caUrl);
                strcpy(caDesc,"\0");
            }
            if(strstr(caUrl,"webclass.proeim.com")) {  /* ��ַ���  */
                ncUrlPutUpdateClass(fpo,sValue.nId,caDesc);
            }
            else if(strstr(caUrl,"updatelog.proeim.com")==NULL) {
                ncUrlPutUpdateUrl(fpo,sValue.nId,caUrl,caDesc);                
            } 
            if(iSetFlags == (-1)) {  /* ������ SetFlags  */
                free(p);
            }
            else {
                pKey[iCount] = p;
            }
            iCount++;
        }
        p = dpiternext(dp,NULL);
    }
    /* �޸� Flags  */
    for(i=0;i<iCount;i++) {
        iReturn = dpgetwb(dp,pKey[i],16,0,sizeof(ncUrlValue),(char *)&sValue);
        if(iReturn > 0) {
            sValue.cFlags = iSetFlags;
            dpput(dp,pKey[i],16,(char *)&sValue,iReturn,DP_DOVER);
        }
        free(pKey[i]);
    }
    fclose(fpo);
    dpclose(dp);
    return iCount;
}


int ncUrlSumLog(DEPOT *dp)
{
    char caUrl[32];
    char caDesc[64];
    long lId=0;
    int iRet;
    int iNum;
    strcpy(caDesc,"0");
    sprintf(caUrl,"%d.updatelog.proeim.com",0);
    iRet =  ncUrlGetUrlInfo0(dp,caUrl,caDesc,&lId);
    if(iRet == 0) {
        iNum = 0;
    }
    else {
        iNum = atol(caDesc);
    }
    return iNum;
}


int ncUrlSaveLog(DEPOT *dp,char *pMsg)
{
    char caUrl[32];
    char caDesc[64];
    long lId=0;
    int iRet;
    int iNum;
    iNum = ncUrlSumLog(dp);
#ifdef LDEBUG1
    printf("SumUrl=%d \n",iNum);
#endif    
    sprintf(caDesc,"%d",iNum+1);
    sprintf(caUrl,"%d.updatelog.proeim.com",0);
    iRet = ncUrlPutUrlInfo0(dp,caUrl,caDesc,0);
#ifdef LDEBUG1
    printf("Url[%s][%s] Num=%d  iRet=%d\n",caUrl,caDesc,iNum+1,iRet);
#endif    
    sprintf(caUrl,"%d.updatelog.proeim.com",iNum+1);
    iRet = ncUrlPutUrlInfo0(dp,caUrl,pMsg,0);
    return iRet;
}


/* �����µ���ַ��   */        
int ncUrlCheckUpdateUrl(char *pDir)
{
    char    caFileName[256];
    struct  stat   st1;
   	struct dirent *d;
    DIR *dir;
    unsigned long lTime;
    int iCount;
    int iReturn;
    iCount=0;
#ifdef LDEBUG1    
    pasLog(PAS_LOG_EVENT,"ncUrlCheckUpdateUrl %s",pDir);
#endif    
    if(dir = opendir(pDir)) {
        while (d = readdir(dir)) {
            lTime = time(0);
		    if (d->d_name[0] == '.') continue;
			sprintf(caFileName, "%s/%s", pDir, d->d_name);
			if (stat(caFileName,&st1) == 0) {
			    if(S_ISREG(st1.st_mode)) {
			        if(st1.st_mtime < lTime - 10) {
			            if(strncmp(d->d_name,"upurl",5) == 0 && strstr(d->d_name,".url")) {
			                char caMsg[128];
			                pasLog(PAS_LOG_EVENT,"ncUrlCvtUpFile2Udb: %s==>/home/ncmysql/nc/conf/url/ncurl.udb",
			                       caFileName);
			                ncUrlCvtUpFile2Udb(caFileName,"/home/ncmysql/nc/conf/url/ncurl.udb",0,NC_URL_UPFLAGS_DONE,caMsg);
			                remove(caFileName);
			                pasLog(PAS_LOG_EVENT,"UpdateUrl,From:%s %s ",d->d_name,caMsg);
			                iCount++;
			            }
			        }
			    }
			}
        }
    }
    return iCount;
}
