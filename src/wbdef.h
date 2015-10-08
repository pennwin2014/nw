#ifndef __WBDEF__
#define __WBDEF__
#include <stdarg.h>
#include "utoall.h"
#include "utoplt01.h"

/*    */
typedef struct wmOnlineUser_S {
    char caSid[24];                 /* Session Id    */
    char caUserName[50];            /*         */
    char caHomeDir[64];             /*         */
    unsigned long lClientIp;        /* IP  */
    unsigned long lLoginTime;       /*       */
    unsigned long lLastTime;        /*   */
    int           iStatus;          /*   0--  1--  */
} wmOnlineUser;


struct wmUsrInfo_S {
    char caSid[24];                 /* SessionId     */
    char caUserName[50];            /*         */
    int  iStatus;                   /*       */
};    

/*   */
typedef struct {
    long lSumNum;                   /*          */
    long lMaxNum;                   /*        */
    unsigned long lTime;            /*          */
    struct wmUsrInfo_S *psSid;      /*    */ 
} wmOnlineLst;

/* 文件夹列表  Add By Liyunming  */
typedef struct {
    unsigned long iFileNum;
    char cname[32];
} wmFlodLst;


#define MIL_LNK_USRLST              21

/*  */
#define WM_ONSTA_REGISTER           1      /*   */
#define WM_ONSTA_LOGIN              0      /*     */

#define WM_ONLINE_CHECKTIME         300    /*   */
#define WM_ONLINE_KEEPTIME          1800   /*       */

#define wbDefTest                   16800     /*           */
#define wbDefRegFirst               16801     /* 1     */
#define wbDefRegLicense             16802     /*   */
#define wbDefRegUserInfo            16803     /*   */
#define wbDefRegLogin               16804     /*       */              
#define wbDefUserLogin			    16901     /**/
#define wbDefLoginOutLeft           16902     /**/	
#define wbDefListFolders            16903     /**/

#define wbDefReadFold               16904     /**/

#define wbDefAddFold                16905     /* 增加文件夹  Add by LiYunming */
#define wbDefDelFold                16906     /* 删除文件夹  Add by Liyunming */

#define wbDefReadFoldHeads          16909     /**/
#define wbDefReadMail               16913     /*Read the mail*/
//#define wbDefPreviewMail            16914     /*Preview the mail*/   /*无此功能*/
#define wbDefMoveMail               16915     /**/
#define wbDefCopyMail               16916     /**/
#define wbDefDeleteMails            16912     /**/
#define wbDefDeleteMail             19912     /*sufan added*/
#define wbDefOutComposePage         16910     /**/
#define wbDefOutComposePage1        19910     /*sufan added*/
#define wbDefOutAttachPage          16932     /**/
#define wbDefSendMail               16930     /*Send the mail*/

/*-----------------------added by ------------------------*/
#define wbDefSaveMail				16935		//
#define wbDefShowPInfo				17601		//
#define wbDefSetPInfo				17602		//
#define wbDefSetSys					17000		//显示系统管理页面
/*-----------------------added by ------------------------*/

#define wbDefFwdModify              17271    /**/
#define wbDefFwdModifyDone          17272    /**/
#define wbDefFilterInfo             17263    /**/
#define wbDefReadAddRefuse          17262    /*  */
#define wbDefFilterNew              17267    /**/
#define wbDefAppendRefuseList       19267    /*sufan added*/
#define wbDefFilterModify           17266    /**/
#define wbDefFilterDone             17265    /*   */
#define wbDefFilterDelete           17264    /**/

#define wbDefAddrInfo               17100    /**/
#define wbDefAddrPerNew             17101    /*  */
#define wbDefAddrNew                19101    /*sufan added  */
#define wbDefAddrPerNewDone         17102    /* ( )*/
#define wbDefAddrPerDelete          17107    /**/
#define wbDefAddrPerCompose         17103    /**/
#define wbDefAddrGrpCompose         17114    /**/
#define wbDefAddrGrpNew             17111    /*  */
#define wbDefAddrGrpNewDone         17112    /*   */
#define wbDefAddrGrpDelete          17117    /**/
#define wbDefAddrPreSave            17109    /**/

#define wbDefPopInfo                17201    /*pop */
#define wbDefPopNewandDone          17202    /*poppop*/
#define wbDefPopModify              17203    /*pop*/
#define wbDefPopDelete              17204    /*pop*/

#define wbDefSignInfo               17301    /**/
#define wbDefSignNewandDone         17302    /**/
#define wbDefSignModify             17303    /**/
#define wbDefSignDelete             17304    /**/
#define wbDefConfigModifyPassword   17401    /**/
#define wbDefConfigModifyPasswordDone   17402    /**/
#define wbDefComposeListAddr            17403    /**/

#define wbDefRegLogout              17501		/**/

#define wbDefForgetpassword_user    19401   /* sufan added*/
#define wbDefForgetpassword_answer  19402   /* sufan added*/ 
#define wbDefForgetpassword_change  19403   /* sufan added*/  
#define wbDefForgetpassword_tologin 19404   /* sufan added*/ 
/* wbutil.c */
int wmUtlSuccessMsg(int iFd,utMsgHead *psMsgHead,
                utPltDbHead *psDbHead,char *pFormat,char *sid);
int wmUtlErrorMsg(int iFd,utMsgHead *psMsgHead,
                utPltDbHead *psDbHead,char *pFormat,...);
int wmUtlBackMsg(int iFd,utMsgHead *psMsgHead,
                utPltDbHead *psDbHead,char *pTitle,char *pFormat,...);                
int wmUtlCloseMsg(int iFd,utMsgHead *psMsgHead,
                utPltDbHead *psDbHead,char *pTitle,char *pFormat,...);
int wmUtlCreateSid(char *caSid);  
wmOnlineUser *wmUtlOnlineUser(utShmHead *psShmHead,char *pSid);
int wmUtlOnlineCheck(char *psShmHead0);
int wmUtlDeleteSid(utShmHead *psShmHead,char *pSid);
int wbUtlNameIsValid(char *pUserName);
int wbCreateFold(char *pHome,char *pFoldName);
int wbDelFold(char *pHome,char *pFoldName);
wmFlodLst *wbReadFoldList(char *pHome,int *iSum);
             
/* wb001.c */
int wbFunRegFirst(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunRegLicense(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunRegUserInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunTest(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/*wb_login.c*/
int wbFunUserLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunLoginOutLeft(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunListFolders(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
 
int wbFunDelFolder(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunAddFolder(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
 
/*  wb101.c   */

int wbFunReadFold(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead); 
int wbFunReadFoldHeads(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunReadMail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
//int wbFunPreviewMail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/*  wb102.c   */ 

int wbFunMoveMail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunCopyMail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunDeleteMails(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/***  wb103.c   ***/

int wbFunOutComposePage(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunOutAttachPage(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunSendMail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/*-----------------------added by ------------------------*/
int wbFunSaveMail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
/*-----------------------added by ------------------------*/


/***  wb104.c   ***/

int wbFunAddrInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunAddrPerNew(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunAddrPerNewDone(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunAddrPerDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunAddrPerCompose(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunAddrGrpCompose(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunAddrGrpNew(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunAddrGrpNewDone(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunAddrGrpDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunAddrPreSave(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

 
/***  wb105.c   ***/ 

int wbFunFwdModify(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunFwdModifyDone(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/***  wb106.c   ***/ 

int wbFunFilterInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunReadAddRefuse(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunFilterNew(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunFilterModify(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunFilterDone(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunFilterDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/***  wb107.c   ***/ 
int wbFunPopInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunPopNewandDone(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunPopModify(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunPopDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/***  wb108.c   ***/ 
int wbFunSignInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunSignNewandDone(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunSignModify(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunSignDelete(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/***  wb109.c   ***/ 
int wbFunShowPInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunSetPInfo(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/***wb201.c    ***/
int wbFunConfigModifyPassword(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunConfigModifyPasswordDone(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/***wb202.c    ***/
int wbFunComposeListAddr(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunSetSys(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/***wb301.c    ***/
int wb_rmAbortionUser(utShmHead *psShmHead, struct wmUsrInfo_S *psSid);	//
int wb_doLogout(utShmHead *psShmHead, char *caSid);	//
int wbFunRegLogout(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/*** wb601.c ******/
/*** sufan added ***/
int wbFunOutComposePage1(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunAddrNew(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunDeleteMail(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunAppendRefuseList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);

/*** wb602.c ******/
/*** sufan added **/
int wbFunForgetpassword_user(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead);
int wbFunForgetpassword_answer(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int wbFunForgetpassword_change(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);
int wbFunForgetpassword_tologin(utShmHead *psShmHead,int iFd,utMsgHead *psMsgHead);

#endif
