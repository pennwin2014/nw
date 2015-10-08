#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include "utoall.h"
#include "pasmysql.h"
#include "ncdef.h"
int iNcClient;
main(int argc, char *argv[])
{
    utShmHead *psShmHead;
    int iReturn;
    int iUp;
    long long lPacket[2],lBytes[2];
    char caIp[32],caMac[32],caMask[32];
    psShmHead = utComShmOpen("pqconf_nc.cnf");
    if(psShmHead == NULL) {
        printf(" Share memory error \n");
        return 0;
    }
    utPltSetPlatePath("/home/ncmysql/nc/plate");

    iReturn = pasConnect(psShmHead);       /* 连接数据库  */
    gdgAppWebSite(psShmHead);
}

