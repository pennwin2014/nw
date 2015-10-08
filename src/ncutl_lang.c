/* 有关词法分析   */
#include <stdio.h>
#include <string.h>
#include "utoall.h"
#include "pasutl.h"
#include "ncdef.h"

#define   NC_MAX_WORD  2048

char *ncLang(char *pStr)
{
    unsigned long lMaxWord;
    char *p;
    unsigned long *psIndex;
    char caTemp[8];
    int iNum;
#ifdef LDEBUG
    printf("ncLang: %s \n",pStr);
#endif
    if(strlen(pStr) < 4) {
        return pStr;
    }
    p = utShmArray(utShmCurHead(),NC_LNK_LANG);
    if(p == NULL) {
        return pStr+4;
    }
    memcpy(&lMaxWord,p,sizeof(unsigned long));
    if(lMaxWord >= NC_MAX_WORD) {
        return pStr+4;
    }
    psIndex = (unsigned long *)((char *)p + 16);
    memcpy(caTemp,pStr,4);
    caTemp[4]=0;
    if(!utStrIsDigital(caTemp)) {
        return pStr;
    }
    else {
        iNum = atol(caTemp);
        if(iNum > lMaxWord - 1) {
            return pStr+4;
        }
        else {
#ifdef LDEBUG
    printf("ncLang: %d %s \n",iNum,p + psIndex[iNum]);
#endif
            return ((char *)p + psIndex[iNum]);
        }
    }
}

                

int ncLangInit(utShmHead *psShmHead)
{
    char *pLang;
    int iReturn;
    char *p,caBuf[256];
    char *pBuf;
    char caFile[128];
    char caNum[12],caWord[256],caOut[256];
    unsigned long *psIndex;
    int iLen;
    unsigned long lBytes,lSize;
    int iMaxWord,iNum;
    FILE *fp;
    lBytes = 0;

    p = utComGetVar_sd(psShmHead,"DefaultCharset","GB2312");
    if(strcasecmp(p,"big5")==0) {
        pasCharsetSetDefault(PAS_CCODE_BIG5);
    }
    else if(strcasecmp(p,"utf8")==0) {
        pasCharsetSetDefault(PAS_CCODE_UTF8);
    }
    
    pLang = utComGetVar_sd(psShmHead,"NcLang","chinese");
/*
    if(strcasecmp(,"big5")==0) {
        pasCharsetSepLangtDefault(PAS_CCODE_BIG5);
    }
    else if(strcasecmp(pLang,"utf8")==0) {
        pasCharsetSetDefault(PAS_CCODE_UTF8);
    }
*/    
    sprintf(caFile,"%s/lang/%s.lng",utComGetVar_sd(psShmHead,"NCHOME","/home/ncmysql/nw"),pLang);

    iMaxWord = 0;
    fp = fopen(caFile,"r");
    if(fp == NULL) {
    	 printf("caFile=%s\n",caFile);
        pasLog(NC_LOG_ERROR,"Language File %s does not exist \n",caFile);
        return (-1);
    }
    p = fgets(caBuf,255,fp);
    while(p) {
       if(!utStrIsSpaces(caBuf) && caBuf[0] != '#') {
            p = utStrSkipSpaces(caBuf);
            p = utStrGetWord(p,caNum,8," ");
            p = utStrSkipSpaces(p);
            p = utStrGetWord(p,caWord,255,"\n\r");
            if(utStrIsDigital(caNum)) {
                iNum = atol(caNum);
                if(iNum < NC_MAX_WORD) {
                    if(iNum > iMaxWord ) iMaxWord = iNum;
                    lBytes += strlen(caWord)+1;
                }
            }
        }
        p = fgets(caBuf,255,fp);
    }
    fclose(fp);

    iMaxWord = iMaxWord + 1;
    lSize = 16+iMaxWord * sizeof(unsigned long) + lBytes;
    utShmFreeArray(psShmHead,NC_LNK_LANG);
    iReturn = utShmInitArray(psShmHead,NC_LNK_LANG,lSize,1);
    if(iReturn != 0) {
    	 printf("Share Memory Error at ncLangLoad \n");
        pasLog(NC_LOG_ERROR,"Share Memory Error at ncLangLoad \n");
        return (-1);
    }

    pBuf = utShmArray(psShmHead,NC_LNK_LANG);
    memset(pBuf,0,lSize);
    psIndex = (unsigned long *)((char *)pBuf + 16);
    memcpy(pBuf,&iMaxWord,sizeof(unsigned long));
    fp = fopen(caFile,"r");
    if(fp == NULL) {
    	 printf("Language File %s does not exist \n",caFile);
        pasLog(NC_LOG_ERROR,"Language File %s does not exist \n",caFile);
        return (-1);
    }
    p = fgets(caBuf,255,fp);
    lBytes = 16 + iMaxWord * sizeof(unsigned long);
    while(p) {
       if(!utStrIsSpaces(caBuf) && caBuf[0] != '#') {
            p = utStrSkipSpaces(caBuf);
            p = utStrGetWord(p,caNum,8," ");
            p = utStrSkipSpaces(p);
            p = utStrGetWord(p,caWord,255,"\n\r");
            if(utStrIsDigital(caNum)) {
                iNum = atol(caNum);
                if(iNum < iMaxWord && iNum >= 0) {
                    psIndex[iNum] = lBytes;
                    iLen = utStrCvtEsc(caWord,caOut);
                    memcpy(pBuf+lBytes,caOut,iLen+1);
                    lBytes += iLen + 1;
                }
            }
        }
        p = fgets(caBuf,255,fp);
    }
    fclose(fp);
    return 0;
}

