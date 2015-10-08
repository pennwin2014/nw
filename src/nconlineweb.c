/*  Pas控制台  
    这是PAS控制台程序。
                    Writen by Liyunming  at 2000/10/3
 */
#include <stdio.h>
#include <stdlib.h>
#include "utoall.h"
#include "pasdef.h"
#include "ncdef.h"
int iDefaultAccess=0;
int main(int argc,char **argv)
{
    utShmHead  *psShmHead;
    char argval;
    char caTime1[64];
    char caConfFile[64];
    short nMaxPages;
    int i,lCount,iFlags=1;
    char   *p;
    ncHttpBuffer *psHttp;
    printf(" Pas Control Terminal Version 3.0...............\n");
    strcpy(caConfFile,"pqconf_nc.cnf");    
    argv++;
    argc--;
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
            case 'x':
            	iFlags = atol(*argv);
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
    psHttp = (ncHttpBuffer *)utShmArray(psShmHead,NC_LNK_WEB);
    lCount=0;    
    printf(" Num Sip  Start Last  Bytes Use Host Url \n");
    for(i=0;i<utShmArrayRecord(psShmHead,NC_LNK_WEB);i++) {
        if(psHttp[i].cUseFlags >= iFlags) {
            strcpy(caTime1,utTimFormat("%Y-%m-%d %H:%M:%S",psHttp[i].lStartTime));
            lCount++;
            printf("[%3d] %15s %12s %12s %8llu  %12s %d %15s %s\n ",i+1,
                utComHostIp(htonl(psHttp[i].lSip)),caTime1,
                utTimFormat("%H:%M:%S",psHttp[i].lLastTime),
                psHttp[i].lBytes,
                ncCvtMac(psHttp[i].caMac),
                psHttp[i].cUseFlags,
                psHttp[i].caHost,
                psHttp[i].caUrl);
        }
    }
    printf(" MaxWeb == %d  Online == %d \n",utShmArrayRecord(psShmHead,NC_LNK_WEB),lCount);
    utShmClose(psShmHead);
    exit(0);
}


