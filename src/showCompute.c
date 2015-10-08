#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utoall.h"
#include "pasdef.h"
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
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "utoall.h"
#include "pasdb.h"
#include "ncdef.h"
#include "pasutl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <utoall.h>
#include "nwdef.h"
#include "pasdb.h"



int showOnlineCompute(utShmHead *psShmHead)
{
    pasHashInfo sHashInfo;
    uchar *pHash;
    int iReturn;
	long count = 0;
	char caMacStr[20]="";
	char caIp[20]="";
	nwCompInfo* psComp;
    pHash =  (unsigned char *)utShmHashHead(psShmHead, NC_LNK_COMPUTE);
    if(pHash) 
	{
        psComp = (nwCompInfo *)pasHashFirst(pHash,&sHashInfo);
        while(psComp) 
		{
			strcpy(caIp,utComHostIp(htonl(psComp->ip)));
           	printf("\n===============compid=%lu,index=%lu===========\n", psComp->compid, count++);
			printf("os=[%s],ver=[%s],cmpname=[%s],mac=[%s],ip=[%s]\n", psComp->os, psComp->version, psComp->compname, 
				pasCvtMacStr(psComp->mac, caMacStr),caIp);
		   
		   printf("uid=[%lu],stime=[%lu],koff=[%lu],ie=[%s],htm=[%lu]\n",psComp->userid,
		   	psComp->stime,psComp->kickoff,psComp->ie,psComp->htime);
		   printf("gid=[%lu],dname=[%s],ctm=[%lu],cmd=[%d],ckey=[%s],status=[%d]\n\n", psComp->groupid,
		   	psComp->devname,psComp->ctime,psComp->cmod,psComp->caKey,
		   	psComp->status);
		   
           psComp = (nwCompInfo *)pasHashNextS(&sHashInfo);
        }
    }
    return 0;
}



int main(int argc, char **argv)
{
    utShmHead  *psShmHead;
    char caTime[32], caThisTime[32];
    char argval;
    char caConfFile[64];
    int i, iReturn;
    unsigned long lTime;
    int iSum;
    printf("in function main");
    strcpy(caConfFile, "pqconf_nc.cnf");
    /*
    argv++;
    argc--;
    while(argc) {
        if(**argv != '-') {
            printf("ncsshowmaclog  [-d configfile] \n");
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
            default:
                printf("ncicauth [-d configfile] \n");
                exit(0);
        }
    }*/
    psShmHead = utComShmOpen(caConfFile);
    if(psShmHead == NULL)
    {
        printf(" Share Memory not Init \n");
        exit(0);
    }
    else
    {
        printf("\nnot null\n");
    }
	//ncsStatMacDevWarn(psShmHead);
    showOnlineCompute(psShmHead);
    utShmClose(psShmHead);
    exit(0);
}

