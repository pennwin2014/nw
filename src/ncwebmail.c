/*  Pas控制台  
    这是PAS控制台程序。
                    Writen by Liyunming  at 2000/10/3
 */
#include <stdio.h>
#include <stdlib.h>
#include "utoall.h"
#include "pasdef.h"
#include "ncdef.h"
int iNcClient = 0;
int main(int argc,char **argv)
{
    utShmHead  *psShmHead;
    char caConfFile[64];
    char caFile[128];
    int iReturn;
    strcpy(caConfFile,"pqconf_nc.cnf");    
    psShmHead=utComShmOpen(caConfFile);
    if(psShmHead == NULL) {
        printf(" Share Memory not Init \n");
        exit(0);
    }
    printf("FilePath:");
    scanf("%s",caFile);
    
    iReturn = pasConnect(psShmHead);
    if(iReturn != 0) {
        printf(" Database not connect ");
        return 0;
    }
 //   ncMilSetDefCharSet(psShmHead);
    iReturn = ncPostFilterInit(psShmHead);
    if(iReturn < 0) {
        utSysLog(" MailFilter Init Error ");
        return 0;
    }
    iReturn = ncPostProcessPostFile(psShmHead,caFile);
    printf(" iReturn==%d \n",iReturn);
    utShmClose(psShmHead);
    exit(0);
}

