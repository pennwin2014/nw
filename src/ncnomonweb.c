/*  Pas控制台  
    这是PAS控制台程序。
                    Writen by Liyunming  at 2000/10/3
 */
#include <stdio.h>
#include <stdlib.h>
#include "utoall.h"
#include "pasdef.h"
#include "ncdef.h"
int testPrintIp(char *pHead,char *pBuffer);
int main(int argc,char **argv)
{
    utShmHead  *psShmHead;
    char argval;
    char caConfFile[64];
    char caTime[32];
    char caIp[32];
    short nMaxPages;
    char *pHash;
    pasHashInfo sHashInfo;
    int i,iFlags=0,iReturn,iCount;
    long lCount,lStart,lNum;
    char   *p;
    ncTcpBuffer *psIp;
    ncIpPkgInfo *psIpPkg;
    printf(" Pas Control Terminal Version 3.0...............\n");
    strcpy(caConfFile,"pqconf_nc.cnf");    
    argv++;
    argc--;
    lCount = 0;
    lStart = 1;
    lNum = 999999L;
    
    while(argc) {
        if(**argv != '-') {
            printf("pasmon [-d configfile] \n");
            exit(0);
        }
        argval = *(*argv + 1);
        argc--;
        argv++;
        switch(argval) {
            case 'd':
                strcpy(caConfFile,(*argv));
                argc--;
                argv++;
                break;
            case 'f':
                iFlags = atol(*argv);
                argc--;
                argv++;
                break;
            case 's':
                lStart = atol(*argv);
                argc--;
                argv++;
                break;
            case 'l':
                lNum = atol(*argv);
                argc--;
                argv++;
                break;
            case 'h':
                printf("pasmon [-d configfile] \n");
                exit(0);
            default:
                printf("pasmon [-d configfile] \n");
                exit(0);
        }
    }
    psShmHead=utComShmOpen(caConfFile);
    if(psShmHead == NULL) {
        printf(" Share Memory not Init \n");
        exit(0);
    }
    
    
    pHash = utShmHashHead(psShmHead,NC_LNK_NOMONWEB);
    if(pHash == NULL) {
        printf(" Internal Error \n");
    }
    else {
        char *psMail;
        psMail = (char *)pasHashFirst(pHash,&sHashInfo);
        iCount = 0;
        while(psMail) {
           iCount++;
           printf("[%d] [%s]\n",iCount,psMail);
           psMail = (ncMailIpList *)pasHashNext(&sHashInfo);
        }
    }
    utShmClose(psShmHead);
    exit(0);
}
