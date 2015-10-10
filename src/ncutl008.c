/* 一些实用程序  */
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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdarg.h>
#include "utoall.h"
#include "ncdef.h"

/*     检查IP地址目前登录出错的次数,每执行一次，该函数的记数器加1
   返回值：
       目前已出错的次数
   AdmLockType = "Ip"  "UserName"
       
 */
typedef struct ncUtlAdmIp_s {
    unsigned long lIp;              /* 客户端IP地址       */
    unsigned char caName[32];       /* 用户名             */
    unsigned long lTime;            /* 最后一次登录时间   */
    unsigned long lCount;           /* 失败次数           */
} ncUtlAdmIp;

#define NC_MAX_ADMIPLIST        200     /* IP地址的数量       */

int ncAdmCheckAdmIp(utShmHead *psShmHead,unsigned char *pName,unsigned long lIp)
{
    ncUtlAdmIp *psAdm;
    int iReturn,i;
    unsigned long lTime;
    unsigned long lLockTime;
    int iLockType;
    if(strcasecmp(utComGetVar_sd(psShmHead,"AdmLockType","Ip"),"Ip")==0) {
        iLockType = 1;
    }
    else {
        iLockType = 2;
    }
    lLockTime = utComGetVar_ld(psShmHead,"UserLockTime",300);
    lTime = time(0);
    psAdm = (ncUtlAdmIp *)utShmArray(psShmHead,NC_LNK_ADMIPLIST);
    if(psAdm == NULL) {
        iReturn = utShmInitArray(psShmHead,NC_LNK_ADMIPLIST,
                         sizeof(ncUtlAdmIp),NC_MAX_ADMIPLIST);
        if(iReturn < 0) {
            return 0;
        }
        else {
            psAdm = (ncUtlAdmIp *)utShmArray(psShmHead,NC_LNK_ADMIPLIST);
            memset(psAdm,0,sizeof(ncUtlAdmIp)*NC_MAX_ADMIPLIST);
        }
    }
    for(i=0;i<NC_MAX_ADMIPLIST;i++) {
        if(psAdm[i].lTime < lTime - lLockTime) {
            psAdm[i].lIp = 0;
            strcpy(psAdm[i].caName,"\0");
            psAdm[i].lTime = 0;
            psAdm[i].lCount = 0;
        }
        else {
            if(iLockType == 1) {
                if(psAdm[i].lIp == lIp) {
                    psAdm[i].lCount++;
                    psAdm[i].lTime = lTime;
                    return psAdm[i].lCount;
                }
            }
            else {
                if(strcasecmp(psAdm[i].caName,pName)==0) {
                    psAdm[i].lCount++;
                    psAdm[i].lTime = lTime;
                    return psAdm[i].lCount;
                }
            }
                
        }
    }
    for(i=0;i<NC_MAX_ADMIPLIST;i++) {
        if(psAdm[i].lTime < lTime - lLockTime) {
            psAdm[i].lIp = lIp;
            strcpy(psAdm[i].caName,pName);
            psAdm[i].lTime = lTime;
            psAdm[i].lCount = 1;
            return psAdm[i].lCount;
        }
   }
   return 0;
}

/* 去掉IP地址，通常在登录成功后操作 

 */
int ncAdmSetAdmIp(utShmHead *psShmHead,unsigned char *pName,unsigned long lIp)
{
    ncUtlAdmIp *psAdm;
    int iReturn,i;
    unsigned long lTime;
    unsigned long lLockTime;
    int      iLockType;
    int      iDo=0;
    lTime = time(0);
    lLockTime = utComGetVar_ld(psShmHead,"UserLockTime",300);
    if(strcasecmp(utComGetVar_sd(psShmHead,"AdmLockType","Ip"),"Ip")==0) {
        iLockType = 1;
    }
    else {
        iLockType = 2;
    }
    
    psAdm = (ncUtlAdmIp *)utShmArray(psShmHead,NC_LNK_ADMIPLIST);
    if(psAdm == NULL) {
        return 0;
    }
  
    for(i=0;i<NC_MAX_ADMIPLIST;i++) {
        if(psAdm[i].lTime < lTime - lLockTime) {
       
            psAdm[i].lIp = 0;
            psAdm[i].lTime = 0;
            psAdm[i].lCount = 0;
        }
        else {
            if(iLockType == 1) {
                if(psAdm[i].lIp == lIp) {
                    strcpy(psAdm[i].caName,"\0");
                    psAdm[i].lIp = 0;
                    psAdm[i].lTime = 0;
                    psAdm[i].lCount = 0;
                    iDo = 1;
                }
            }
            else {
                if(strcasecmp(psAdm[i].caName,pName)==0) {
                    strcpy(psAdm[i].caName,"\0");
                    psAdm[i].lIp = 0;
                    psAdm[i].lTime = 0;
                    psAdm[i].lCount = 0;
                    iDo = 1;
                }
            }
        }
    }
    return iDo;
}

/* 往操作日志文件中写日志信息   */
int ncAdmWriteLog(char *pName,char *pIp,char *pMsg,...)
{
    char caMessage[256];
    char caDate[20];
    int iReturn;
    unsigned long lSid;
    va_list args;
    va_start(args,pMsg);
    strcpy(caMessage,"\0");
    vsprintf(caMessage,pMsg,args);
    va_end(args);
    strcpy(caDate,utTimFormat("%Y-%m-%d %H:%M:%S",time(0)));
    lSid = pasGetSid("ncadminlog","sid");
    iReturn = pasDbExecSqlF("insert into %s(sid,sdate,name,ipaddr,descr,action) values (%lu,'%s','%s','%s','%s','02')",
            getNewTable(getLoginShortName(),"ncadminlog"),lSid,caDate,pName,pIp,caMessage);
    if(iReturn == 0) {
        pasDbCommit(NULL);
    }
    return 0; 
}



int ncAdmShowAdmIp(utShmHead *psShmHead)
{
    ncUtlAdmIp *psAdm;
    int i,iCount;
    printf(" Now:%s      TimeOut:%lu  LockType:%s\n",
          utTimFormat("%Y-%m-%d %H:%M:%S",time(0)),
          utComGetVar_ld(psShmHead,"UserLockTime",300),
          utComGetVar_sd(psShmHead,"AdmLockType","Ip"));
    psAdm = (ncUtlAdmIp *)utShmArray(psShmHead,NC_LNK_ADMIPLIST);
    if(psAdm == NULL) {
        printf(" Proeim does not start \n");
        return 0;
    }
    iCount = 0;
    for(i=0;i<NC_MAX_ADMIPLIST;i++) {
        if(psAdm[i].lIp != 0) {
            iCount ++;
            printf("[%-3d]Ip:%-15s   User:%-31s   Count:%d   LastTime:%s\n",
                iCount,
                utComHostIp(psAdm[i].lIp),
                psAdm[i].caName,
                psAdm[i].lCount,
                utTimFormat("%Y-%m-%d %H:%M:%S",psAdm[i].lTime));
        } 
    }
   return 0;
}


