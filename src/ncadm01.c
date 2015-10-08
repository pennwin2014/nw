#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
//#include <pcap.h>
#include "ncdef.h"
#include "dsutl.h"
#include "dsclt.h"
//#include "nccltdef.h"


/* 显示增加/修改用户的界面  */
int ncWebAdminDispUserForm(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    unsigned long lId, lGroupid, lTime, lTime1, lUserid, lLevel2;
    char caName[32], caGroup[32], caDispname[32], caEmail[64], caUserid[16];
    char caWhere[256], caRoleid[16], caGroupid[16];
    char *p;
    char caResult[16000], caResult1[8192], caSmt[256];
    utPltDbHead *psDbHead;
    char *pBuf, *pSelect, caLevel[12];
    int iReturn, iRetSum, i, j, k, iSum1, iSum2, iNum, iReturn1;
    long lLevel;
    iReturn = dsCltGetMyInfo(1, "usrlevel", &lLevel);
    lLevel2 = lLevel;
#ifdef LDEBUG
    printf(" lLevel==%d iReturn = %d\n", lLevel, iReturn);
    utMsgPrintMsg(psMsgHead);
#endif
    if((lLevel != 1) && (lLevel != 2) && (lLevel != 0) && (lLevel != 10))
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0664系统管理"), ncLang("0137不好意思，只有系统管理员才可以使用此功能"));
        return 0;
    }
    psDbHead = utPltInitDb();
    iReturn = utMsgGetSomeNVar(psMsgHead, 1,
                               "userid",    UT_TYPE_STRING, 15, caUserid);
    lUserid = atol(caUserid);
    if(!utStrIsSpaces(caUserid))   /* 修改 */
    {
        lUserid = atol(caUserid);
        sprintf(caWhere, "id=%lu", lUserid);
        iReturn = dsCltUserList(1, 1, &iRetSum, &iSum1,
                                "id,name,dispname,groupname,groupid,addtime,moditime,email,usrlevel",
                                caWhere, "id", caResult);
        if(iReturn == 0)
        {
            p = utStrGetRecordValue(caResult, 9, UT_TYPE_LONG, 4, &lUserid,
                                    UT_TYPE_STRING, 31, caName,
                                    UT_TYPE_STRING, 31, caDispname,
                                    UT_TYPE_STRING, 31, caGroup,
                                    UT_TYPE_LONG,   4, &lGroupid,
                                    UT_TYPE_LONG,   4, &lTime,
                                    UT_TYPE_LONG,   4, &lTime1,
                                    UT_TYPE_STRING, 63, caEmail,
                                    UT_TYPE_LONG,   4, &lLevel);
            if(p)
            {
                utPltPutVarF(psDbHead, "userid", "%lu", lUserid);
                utPltPutVar(psDbHead, "name",  caName);
                utPltPutVar(psDbHead, "dispname",  caDispname);
                utPltPutVar(psDbHead, "groupname", caGroup);
                utPltPutVarF(psDbHead, "groupid", "%lu", lGroupid);
                utPltPutVar(psDbHead, "email",     caEmail);
                utPltPutVar(psDbHead, "addtime",   utTimFormat("%Y-%m-%d %H:%M:%S", lTime));
                utPltPutVar(psDbHead, "moditime",   utTimFormat("%Y-%m-%d %H:%M:%S", lTime1));
                utPltPutVarF(psDbHead, "usrlevel", "%lu", lLevel);
                utPltPutVar(psDbHead, "levelname", (char *)dsCltUsrLevel(lLevel));
            }
            else
            {
                utPltFreeDb(psDbHead);
                utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0839增加用户"), ncLang("0773用户信息不正确"));
                return 0;
            }
        }
        else
        {
            utPltFreeDb(psDbHead);
            utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0839增加用户"), ncLang("0743用户不存在 %d"), iReturn);
            return 0;
        }
    }
    else   /* 增加新记录  */
    {
        utPltPutVarF(psDbHead, "groupid", "%lu", 0);
        utPltPutVar(psDbHead, "groupname", "\0");
    }
    /* 显示组信息  */
    sprintf(caGroupid, "%lu", lGroupid);
    pBuf = pasUtlCvtSqlSelectTreeBmA(ncLang("0455全部,0;"));
    //  pBuf = pasUtlCvtSqlSelectA("select groupname,groupid from ncgroup where groupid >0  order by groupname",
    //              ncLang("0451全部,0;"));
    if(pBuf)
    {
        pSelect = pasUtlCvtInputSelect("groupid", caGroupid, pBuf);
        free(pBuf);
        utPltPutVar(psDbHead, "sgroup", pSelect);
        free(pSelect);
    }
    sprintf(caLevel, "%d", lLevel);
    if(lLevel2 == 1)
    {
        pSelect = pasUtlCvtInputSelect("usrlevel", caLevel, "系统管理员,1;一般操作员,3;酒店前台用户,9;VPN管理员,8;");


    }
    else if(lLevel2 == 0)
    {
        pSelect = pasUtlCvtInputSelect("usrlevel", caLevel, "日志管理员,0;");
    }
    else if(lLevel2 == 9)
    {
        pSelect = pasUtlCvtInputSelect("usrlevel", caLevel, "酒店前台管理员,9;");
    }
    else if(lLevel2 == 8)
    {
        pSelect = pasUtlCvtInputSelect("usrlevel", caLevel, "VPN管理员,8;");
    }
    else if(lLevel2 == 10)
    {
        pSelect = pasUtlCvtInputSelect("usrlevel", caLevel, "普通管理员,10;");

    }
    else
        pSelect = pasUtlCvtInputSelect("usrlevel", caLevel, ncLang("0665系统管理员,1;一般操作员,3;"));

    utPltPutVar(psDbHead, "slevel", pSelect);
    free(pSelect);

    //  if(lLevel2!=10){
    /* 显示角色信息  */
    if(lUserid == 0)
    {
        iSum1 = 0;
    }
    else
    {
        sprintf(caSmt, "select roleid from dsuserrole where id= %lu ", lUserid);
        iReturn = dsCltSqlRecordSet(caSmt, 1, 9999, &iSum1, caResult1);
        if(iReturn != 0) iSum1 = 0;
    }
    sprintf(caSmt, "select id,dispname from dsrole ");
    iReturn = dsCltSqlRecordSet(caSmt, 1, 9999, &iSum2, caResult);
    if(iReturn != 0) iSum2 = 0;
    iNum = 0;
    j = 0;

    p = caResult;
    utPltPutVarF(psDbHead, "sumrole", "%d", iSum2);
    for(k = 0; k < iSum2; k++)
    {
        p = utStrGetRecordValue(p, 2,
                                UT_TYPE_STRING, 12, caRoleid,
                                UT_TYPE_STRING, 31, caDispname);
        if(j == 0)
        {
            iNum++;
            utPltPutLoopVarF(psDbHead, "vara", iNum, "v%d", k);
            utPltPutLoopVar(psDbHead, "rolea", iNum, caRoleid);
            utPltPutLoopVar(psDbHead, "labela", iNum, caDispname);
            if(iSum1 > 0 && utStrIsExistRecord(caResult1, 1, 1, caRoleid))
            {
                utPltPutLoopVar(psDbHead, "checka", iNum, "checked");
            }
            j++;
        }
        else if(j == 1)
        {
            utPltPutLoopVarF(psDbHead, "varb", iNum, "v%d", k);
            utPltPutLoopVar(psDbHead, "roleb", iNum, caRoleid);
            utPltPutLoopVar(psDbHead, "labelb", iNum, caDispname);
            if(iSum1 > 0 && utStrIsExistRecord(caResult1, 1, 1, caRoleid))
            {
                utPltPutLoopVar(psDbHead, "checkb", iNum, "checked");
            }
            j = 0;
        }
    }
    utPltPutVarF(psDbHead, "sumright", "%lu", iSum2);
    if(j == 1)
    {
        utPltPutLoopVar(psDbHead, "roleb", iNum, "noneb");
        utPltPutLoopVar(psDbHead, "dispnameb", iNum, ncLang("0818预留"));
        utPltPutLoopVar(psDbHead, "checkb", iNum, "disabled");
    }

    if(strcasecmp(utComGetVar_sd(psShmHead, "rightseperate", "No"), "YES") == 0)
    {
        //  printf("lLevel2=%d\n",lLevel2);
        if(lLevel2 != 0)
        {
            utPltPutVar(psDbHead, "disprol", "none");
        }

    }
    //}
    utPltPutVar(psDbHead, "plen", utComGetVar_sd(psShmHead, "plen", "8"));


    if(strcasecmp(utComGetVar_sd(psShmHead, "secondpasswd", "No"), "YES") == 0)
    {
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/ncadminuser_1.htm");
    }
    else
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/ncadminuser.htm");
    return 0;
}


/* 增加或修改记录信息  */
int ncAdminUserSave(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char caUserid[16];
    char caUsername[32];
    char caPassword1[32], caPassword2[32], caIp[16];
    char caGroupname[32], caDispname[32], caGroupid[16];
    char caLevel[16], caMyname[32];
    char caEmail[64];
    char caKey[32], caKey0[64];
    char caSum[16];
    char caSql[8192];
    char caVar[32];
    char caKey2[64], caKey22[64];
    char *p;
    unsigned long lTime, lUserid, lUsrlevel, lGroupid, lRoleid;
    int iReturn, i, iSum;
    long lLevel;
    iReturn = dsCltGetMyInfo(2, "usrlevel", &lLevel,
                             "username", caMyname);

    dsCltGetSessionValue(1, "dispname", UT_TYPE_STRING, 16, caMyname);
#ifdef LDEBUG
    printf(" lLevel==%d iReturn = %d\n", lLevel, iReturn);
    utMsgPrintMsg(psMsgHead);
#endif
    if((lLevel != 1) && (lLevel != 2) && (lLevel != 0) && lLevel != 10)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0664系统管理"), ncLang("0137不好意思，只有系统管理员才可以使用此功能"));
        return 0;
    }

    lTime = time(0);
    iReturn = utMsgGetSomeNVar(psMsgHead, 12,
                               "userid",      UT_TYPE_STRING, 10, caUserid,
                               "name",        UT_TYPE_STRING, 31, caUsername,
                               "dispname",    UT_TYPE_STRING, 31, caDispname,
                               "password",    UT_TYPE_STRING, 31, caPassword1,
                               "password2",   UT_TYPE_STRING, 31, caPassword2,
                               "key2",        UT_TYPE_STRING, 31, caKey2,
                               "key22",       UT_TYPE_STRING, 31, caKey22,
                               "groupid",     UT_TYPE_STRING, 10, caGroupid,
                               "usrlevel",    UT_TYPE_STRING, 31, caLevel,
                               "email",       UT_TYPE_STRING, 63, caEmail,
                               "sumright",    UT_TYPE_STRING, 10, caSum,
                               "clientip",    UT_TYPE_STRING, 15, caIp);
    lUserid = atol(caUserid);
    lGroupid = atol(caGroupid);
    iSum = atol(caSum);
    if(lGroupid == 0)
    {
        strcpy(caGroupname, ncLang("0446全部"));
    }
    else
    {
        iReturn = pasDbOneRecord("select groupname from ncgroup where groupid = :gid", 1,
                                 "gid", UT_TYPE_LONG, lGroupid,
                                 UT_TYPE_STRING, 31, caGroupname);
        if(iReturn != 0)
        {
            utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0839增加用户"), ncLang("0900组不存在"));
            return 0;
        }
    }
    lUsrlevel = atol(caLevel);
    if(strcmp(caPassword1, caPassword2) != 0)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0839增加用户"), ncLang("0351口令不相等"));
        return 0;
    }

    if(strcmp(caKey2, caKey22) != 0)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0839增加用户"), ncLang("1556第二密码不相等 "));
        return 0;
    }


    if(strcasecmp(utComGetVar_sd(psShmHead, "secondpasswd", "No"), "YES") == 0)
    {


        if(utStrIsSpaces(caUserid) || lUserid == 0)   /* 新增记录  */
        {
            lUserid = dsCltGetSeque();
            if(lUserid > 0)
            {
                if(utStrIsSpaces(caPassword1))
                {
                    utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0839增加用户"), ncLang("0349口令不能为空"));
                    return 0;
                }

                sprintf(caKey0, "%s%s", caUsername, caPassword1);
                utMd5Ascii22(caKey0, strlen(caKey0), NULL, caKey); /* 口令加密  */
                sprintf(caSql, "<DSSQL>insert into dsuser (id,name,dispname,groupid,groupname,addtime,\
                              moditime,lkey,lasttime,logcount,useflags,status,usrlevel,email,key2) values \
                           (%lu,'%s','%s',%lu,'%s',%lu,%lu,'%s',%lu,0,0,0,%d,'%s','%s')</DSSQL>\n",
                        lUserid, caUsername, caDispname, lGroupid, caGroupname, lTime, lTime,
                        caKey, lTime, lUsrlevel, caEmail, caKey2);

                for(i = 0; i < iSum; i++)
                {
                    sprintf(caVar, "v%d", i);
                    p = utMsgGetVar_s(psMsgHead, caVar);
                    if(p)
                    {
                        lRoleid = atol(p);
                        utStrAddF(caSql, "<DSSQL>insert into dsuserrole(id,roleid,addtime,addby) values \
                              (%lu,%lu,%lu,'%s')</DSSQL>\n",
                                  lUserid, lRoleid, lTime, caMyname);
                    }
                }
                iReturn = dsCltSqlExecSome(caSql);
                if(iReturn == 0)
                {
                    ncWebAdminUserList(psShmHead, iFd, psMsgHead);
                    //                ncAdmWriteLog(caMyname,caIp,ncLang("0689新增一个用户 %s"),caUsername);
                    char caMsg[128];
                    sprintf(caMsg, caIp, ncLang("0689新增一个用户 %s"), caUsername);
                    // ncsWriteSysLog(psMsgHead,caMyname,"02","Success",caMsg);
                    writeSysLog(psMsgHead, "02", "Success", caMsg);

                }
                else
                {
                    utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0839增加用户"), ncLang("0780用户增加出错"));
                }

            }
            else
            {
                utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0839增加用户"), ncLang("0780用户增加出错"));
                return 0;
            }
        }
        else   /* 修改记录  */
        {
            sprintf(caSql, "<DSSQL>update dsuser set name='%s',dispname='%s',groupid=%lu,\
                            groupname='%s'",
                    caUsername, caDispname, lGroupid, caGroupname);
            if(!utStrIsSpaces(caPassword1))
            {
                sprintf(caKey0, "%s%s", caUsername, caPassword1);
                utMd5Ascii22(caKey0, strlen(caKey0), NULL, caKey); /* 口令加密  */
                utStrAddF(caSql, ",lkey='%s'", caKey);
            }

            if(!utStrIsSpaces(caKey2))
            {

                utStrAddF(caSql, ",key2='%s'", caKey2);
            }


            utStrAddF(caSql, ",moditime=%lu,lasttime=%lu,usrlevel=%lu,email='%s' where id=%lu </DSSQL>\n",
                      lTime, lTime, lUsrlevel, caEmail, lUserid);


            utStrAddF(caSql, "<DSSQL>delete from dsuserrole where id = %lu </DSSQL>\n", lUserid);
            for(i = 0; i < iSum; i++)
            {
                sprintf(caVar, "v%d", i);
                p = utMsgGetVar_s(psMsgHead, caVar);
                if(p)
                {
                    lRoleid = atol(p);
                    utStrAddF(caSql, "<DSSQL>insert into dsuserrole(id,roleid,addtime,addby) values \
                          (%lu,%lu,%lu,'%s')</DSSQL>\n",
                              lUserid, lRoleid, lTime, caMyname);
                }
            }
            iReturn = dsCltSqlExecSome(caSql);
            if(iReturn == 0)
            {
                ncWebAdminUserList(psShmHead, iFd, psMsgHead);
                ncAdmWriteLog(caMyname, caIp, ncLang("0706修改用户[%s]的信息"), caUsername);

            }
            else
            {
                utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0839增加用户"), ncLang("0777用户修改错误%d"), iReturn);
            }
        }

    }
    else
    {




        if(utStrIsSpaces(caUserid) || lUserid == 0)   /* 新增记录  */
        {
            lUserid = dsCltGetSeque();
            if(lUserid > 0)
            {
                if(utStrIsSpaces(caPassword1))
                {
                    utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0839增加用户"), ncLang("0349口令不能为空"));
                    return 0;
                }

                sprintf(caKey0, "%s%s", caUsername, caPassword1);
                utMd5Ascii22(caKey0, strlen(caKey0), NULL, caKey); /* 口令加密  */
                sprintf(caSql, "<DSSQL>insert into dsuser (id,name,dispname,groupid,groupname,addtime,\
                              moditime,lkey,lasttime,logcount,useflags,status,usrlevel,email) values \
                           (%lu,'%s','%s',%lu,'%s',%lu,%lu,'%s',%lu,0,0,0,%d,'%s')</DSSQL>\n",
                        lUserid, caUsername, caDispname, lGroupid, caGroupname, lTime, lTime,
                        caKey, lTime, lUsrlevel, caEmail);
                for(i = 0; i < iSum; i++)
                {
                    sprintf(caVar, "v%d", i);
                    p = utMsgGetVar_s(psMsgHead, caVar);
                    if(p)
                    {
                        lRoleid = atol(p);
                        utStrAddF(caSql, "<DSSQL>insert into dsuserrole(id,roleid,addtime,addby) values \
                              (%lu,%lu,%lu,'%s')</DSSQL>\n",
                                  lUserid, lRoleid, lTime, caMyname);
                    }
                }
                iReturn = dsCltSqlExecSome(caSql);
                if(iReturn == 0)
                {
                    ncWebAdminUserList(psShmHead, iFd, psMsgHead);
                    ncAdmWriteLog(caMyname, caIp, ncLang("0689新增一个用户 %s"), caUsername);
                }
                else
                {
                    utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0839增加用户"), ncLang("0780用户增加出错"));
                }

            }
            else
            {
                utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0839增加用户"), ncLang("0780用户增加出错"));
                return 0;
            }
        }
        else   /* 修改记录  */
        {
            sprintf(caSql, "<DSSQL>update dsuser set name='%s',dispname='%s',groupid=%lu,\
                            groupname='%s'",
                    caUsername, caDispname, lGroupid, caGroupname);
            if(!utStrIsSpaces(caPassword1))
            {
                sprintf(caKey0, "%s%s", caUsername, caPassword1);
                utMd5Ascii22(caKey0, strlen(caKey0), NULL, caKey); /* 口令加密  */
                utStrAddF(caSql, ",lkey='%s'", caKey);
            }
            utStrAddF(caSql, ",moditime=%lu,lasttime=%lu,usrlevel=%lu,email='%s' where id=%lu </DSSQL>\n",
                      lTime, lTime, lUsrlevel, caEmail, lUserid);
            utStrAddF(caSql, "<DSSQL>delete from dsuserrole where id = %lu </DSSQL>\n", lUserid);
            for(i = 0; i < iSum; i++)
            {
                sprintf(caVar, "v%d", i);
                p = utMsgGetVar_s(psMsgHead, caVar);
                if(p)
                {
                    lRoleid = atol(p);
                    utStrAddF(caSql, "<DSSQL>insert into dsuserrole(id,roleid,addtime,addby) values \
                          (%lu,%lu,%lu,'%s')</DSSQL>\n",
                              lUserid, lRoleid, lTime, caMyname);
                }
            }
            iReturn = dsCltSqlExecSome(caSql);
            if(iReturn == 0)
            {
                ncWebAdminUserList(psShmHead, iFd, psMsgHead);
                ncAdmWriteLog(caMyname, caIp, ncLang("0706修改用户[%s]的信息"), caUsername);

            }
            else
            {
                utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_space.htm", ncLang("0839增加用户"), ncLang("0777用户修改错误%d"), iReturn);
            }
        }


    }
    return 0;
}


/* 显示角色的权限   */
int ncWebAdminRoleList(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    dsCltStatus *psMyStatus;
    unsigned long lRoleid, lPasid;
    char caRoleid[16], caPasid[16];
    int iReturn;
    long lLevel;
    iReturn = dsCltGetMyInfo(1, "usrlevel", &lLevel);
#ifdef LDEBUG
    printf(" lLevel==%d iReturn = %d\n", lLevel, iReturn);
    utMsgPrintMsg(psMsgHead);
#endif
    if(lLevel != 1)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0664系统管理"), ncLang("0137不好意思，只有系统管理员才可以使用此功能"));
        return 0;
    }

    psMyStatus = (dsCltStatus *)utShmArray((utShmHead *)psShmHead, UT_LNK_MYSTATUS);
    if(psMyStatus == NULL)
    {
        return 0;
    }
    lPasid  = psMyStatus->lId;
    iReturn = utMsgGetSomeNVar(psMsgHead, 1,
                               "roleid", UT_TYPE_STRING, 12, caRoleid);
    lRoleid = atol(caRoleid);
    psDbHead = utPltInitDb();

    utPltPutVarF(psDbHead, "roleid", "%lu", lRoleid);
    utPltPutVarF(psDbHead, "pasid",  "%lu", lPasid);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "ds/ncadminrolemenu.htm");
    return 0;
}


/* 增加或修改记录信息  */
int ncAdminRoleModi(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    dsCltStatus *psMyStatus;
    long lId;
    unsigned long lTime;
    char caAdd[12], caBak[12], caId[12];
    char caSql[1024];
    int iReturn;
    char caName[32], caMyname[32];
    char caDispname[32], caDescr[64];
    char caIp[16];
    char *p, *p1;
    long lLevel;
    iReturn = dsCltGetMyInfo(2, "usrlevel", &lLevel,
                             "username", caMyname);
#ifdef LDEBUG
    printf(" lLevel==%d iReturn = %d\n", lLevel, iReturn);
    utMsgPrintMsg(psMsgHead);
#endif
    if(lLevel != 1)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0664系统管理"), ncLang("0137不好意思，只有系统管理员才可以使用此功能"));
        return 0;
    }

    lTime = time(0);
    iReturn = utMsgGetSomeNVar(psMsgHead, 7,
                               "add",      UT_TYPE_STRING, 10, caAdd,
                               "bak",      UT_TYPE_STRING, 10, caBak,
                               "id",       UT_TYPE_STRING, 10, caId,
                               "name",     UT_TYPE_STRING, 31, caName,
                               "dispname", UT_TYPE_STRING, 31, caDispname,
                               "clientip",  UT_TYPE_STRING, 15, caIp,
                               "descr",    UT_TYPE_STRING, 31, caDescr);
    if(!utStrIsSpaces(caBak))
    {
        dsCltWebDsroleList(psShmHead, iFd, psMsgHead);
        return 0;
    }
    psMyStatus = (dsCltStatus *)utShmArray((utShmHead *)psShmHead, UT_LNK_MYSTATUS);
    if(psMyStatus == NULL)
    {
        return 0;
    }
    lId = atol(caId);
    if(!utStrIsSpaces(caAdd) || lId == 0)   /* 新增记录  */
    {
        lId = dsCltGetSeque();
        if(lId > 0)
        {
            sprintf(caSql, "<DSSQL>insert into dsrole (id,name,dispname,descr,addtime,moditime) values ( \
                    %lu,'%s','%s','%s',%lu,%lu) </DSSQL>\n \
                    <DSSQL>insert into dspasrole(roleid,pasid) values (%lu,%lu) </DSSQL>",
                    lId, caName, caDispname, caDescr, lTime, lTime, lId, psMyStatus->lId);
            iReturn = dsCltSqlExecSome(caSql);
            if(iReturn == 0)
            {
                dsCltWebDsroleList(psShmHead, iFd, psMsgHead);
                ncAdmWriteLog(caMyname, caIp, ncLang("0688新增权限%s"), caDescr);
            }
            else
            {
                utWebDispMsg(iFd, psMsgHead, "ds/dsmsg_back.htm", ncLang("0839增加用户"),
                             ncLang("0318记录增加失败   [%d]"), iReturn);
            }
        }
        else
        {
            utWebDispMsg(iFd, psMsgHead, "ds/dsmsg_back.htm", ncLang("0839增加用户"),
                         ncLang("0317记录增加失败"));
        }
    }
    else   /* 修改记录  */
    {
        sprintf(caSql, "<DSSQL>update dsrole set name='%s',dispname='%s',descr='%s',moditime='%lu' where id=%lu </DSSQL>",
                caName, caDispname, caDescr, lTime, lId);
        iReturn = dsCltSqlExecSome(caSql);
        if(iReturn == 0)
        {
            dsCltWebDsroleList(psShmHead, iFd, psMsgHead);
            ncAdmWriteLog(caMyname, caIp, ncLang("0702修改权限%s"), caDescr);
        }
        else
        {
            utWebDispMsg(iFd, psMsgHead, "ds/dsmsg_back.htm", ncLang("0839增加用户"),
                         ncLang("0309记录修改失败   [%d]"), iReturn);
        }
    }
    return 0;
}




/* 显示角色的权限   */
int ncWebAdminRole(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    utPltDbHead *psDbHead;
    dsCltStatus *psMyStatus;
    unsigned long lRoleid, lPasid;
    char caRoleid[16], caPasid[16];
    int iReturn;
    long lLevel;
    iReturn = dsCltGetMyInfo(1, "usrlevel", &lLevel);
#ifdef LDEBUG
    printf(" lLevel==%d iReturn = %d\n", lLevel, iReturn);
    utMsgPrintMsg(psMsgHead);
#endif
    if(lLevel != 1)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0664系统管理"), ncLang("0137不好意思，只有系统管理员才可以使用此功能"));
        return 0;
    }

    psMyStatus = (dsCltStatus *)utShmArray((utShmHead *)psShmHead, UT_LNK_MYSTATUS);
    if(psMyStatus == NULL)
    {
        return 0;
    }
    lPasid  = psMyStatus->lId;
    iReturn = utMsgGetSomeNVar(psMsgHead, 1,
                               "roleid", UT_TYPE_STRING, 12, caRoleid);
    lRoleid = atol(caRoleid);
    psDbHead = utPltInitDb();

    utPltPutVarF(psDbHead, "roleid", "%lu", lRoleid);
    utPltPutVarF(psDbHead, "pasid",  "%lu", lPasid);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "ds/nc_dsrole.htm");
    return 0;
}


/* 系统管理   */
int ncWebAdminUserList(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char caResult[16000], caWhere[512], caCond[64], caCond0[64];
    char caName[32], caDispname[32], caGroup[32], caEmail[64];
    char caCurpage[13], caPerrow[13], caTotpg[13], caDel[16];
    utPltDbHead *psDbHead;
    int iReturn, iNum, i;
    char caMyname[32];
    char caIp[16];
    int iStart, iSum, iRetSum, iTotal;
    unsigned long lId, lTime, lTotal, lCurpg, lLevel;
    char *p;
    iReturn = dsCltGetMyInfo(2, "usrlevel", &lLevel,
                             "username", caMyname);
#ifdef LDEBUG
    printf(" lLevel==%d iReturn = %d\n", lLevel, iReturn);
    utMsgPrintMsg(psMsgHead);
#endif
    if((lLevel != 1) && (lLevel != 2) && (lLevel != 0) && (lLevel != 10))
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0664系统管理"), ncLang("0137不好意思，只有系统管理员才可以使用此功能"));
        return 0;
    }

    //     printf("pid=%d\n",getpid());
    //     sleep(20);
    iReturn = utMsgGetSomeNVar(psMsgHead, 6,
                               "cond",  UT_TYPE_STRING, 63, caCond,
                               "CurPg", UT_TYPE_STRING, 12, caCurpage,
                               "TotPg", UT_TYPE_STRING, 12, caTotpg,
                               "PerRow", UT_TYPE_STRING, 12, caPerrow,
                               "del",   UT_TYPE_STRING, 6,  caDel,
                               "clientip", UT_TYPE_STRING, 15, caIp);
    if(!utStrIsSpaces(caDel))
    {
        char caVar[16];
        for(i = 0; i < 35; i++)
        {
            sprintf(caVar, "chk%d", i + 1);
            p = utMsgGetVar_s(psMsgHead, caVar);
            if(p)
            {
                lId = atol(p);
                if(lId == 100)
                {
                    utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0664系统管理"), ncLang("0090admin是超级用户，不能删除"));
                    return 0;
                }
                iReturn = dsCltUserDel(atol(p));
                ncAdmWriteLog(caMyname, caIp, ncLang("0490删除用户,ID为 %s"), p);
            }
        }
    }
    lCurpg = atol(caCurpage);
    if(lCurpg == 0) lCurpg = 1;
    if(utStrIsSpaces(caTotpg))
    {
        lTotal = 0;
    }
    else
    {
        lTotal = atol(caTotpg);
    }
    iSum = atol(caPerrow);
    if(iSum <= 0) iSum = 20;
    iStart = (lCurpg - 1) * iSum + 1;
    sprintf(caCond0, "'%c%s%c'", '%', caCond, '%');
    if(!utStrIsSpaces(caCond))
    {

        if(lLevel == 1)
        {
            sprintf(caWhere, " (name like %s or groupname like %s or dispname like %s or email like %s) and usrlevel>= %lu and usrlevel!=0 ",
                    caCond0, caCond0, caCond0, caCond0, lLevel);
        }
        else
        {
            sprintf(caWhere, " (name like %s or groupname like %s or dispname like %s or email like %s) and usrlevel= %lu  ",
                    caCond0, caCond0, caCond0, caCond0, lLevel);
        }
    }
    else
    {
        if(lLevel == 1)
        {
            sprintf(caWhere, "  usrlevel>= %lu and usrlevel!=0 ", lLevel);
        }
        else
        {
            sprintf(caWhere, "  usrlevel= %lu ", lLevel);
        }
    }
    if(lTotal == 0)
    {
        iTotal = 0;
        iReturn = dsCltUserList(iStart, iSum, &iRetSum, &iTotal,
                                "id,name,dispname,groupname,addtime,email,usrlevel",
                                caWhere,
                                "id",
                                caResult);
        if(iReturn == 0) lTotal = (iTotal - 1) / iSum + 1;
    }
    else
    {
        iReturn = dsCltUserList(iStart, iSum, &iRetSum, NULL,
                                "id,name,dispname,groupname,addtime,email,usrlevel",
                                caWhere,
                                "id",
                                caResult);
    }
    if(iReturn != 0)
    {
        iRetSum = 0;
    }
    psDbHead = utPltInitDb();
    if(lLevel == 0)
    {
        utPltPutVarF(psDbHead, "disabled", "disabled");
    }

    utPltPutVarF(psDbHead, "CurPg", "%lu", lCurpg);
    utPltPutVarF(psDbHead, "TotPg", "%lu", lTotal);
    utPltPutVarF(psDbHead, "PerRow", "%lu", iSum);
    utPltPutVar(psDbHead, "cond", caCond);
    p = caResult;
    for(iNum = 1; iNum <= iRetSum; iNum++)
    {
        p = utStrGetRecordValue(p, 7, UT_TYPE_LONG, 4, &lId,
                                UT_TYPE_STRING, 31, caName,
                                UT_TYPE_STRING, 31, caDispname,
                                UT_TYPE_STRING, 31, caGroup,
                                UT_TYPE_LONG,   4, &lTime,
                                UT_TYPE_STRING, 63, caEmail,
                                UT_TYPE_LONG, 4, &lLevel);
        if(p)
        {
            utPltPutLoopVarF(psDbHead, "num", iNum, "%lu", iNum);
            utPltPutLoopVarF(psDbHead, "userid", iNum, "%lu", lId);
            utPltPutLoopVar(psDbHead, "username", iNum, caName);
            utPltPutLoopVar(psDbHead, "dispname", iNum, caDispname);
            utPltPutLoopVar(psDbHead, "groupname", iNum, caGroup);
            utPltPutLoopVar(psDbHead, "email", iNum, caEmail);
            utPltPutLoopVar(psDbHead, "addtime", iNum, utTimFormat("%Y-%m-%d %H:%M:%S", lTime));
            //           utPltPutLoopVar(psDbHead,"level",iNum,nccvtUsrLevel(lLevel));
        }
    }
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/ncadminuserlist.htm");
    return 0;
}




extern struct ncPltMenu_s
{
    int    iFlags;
    char   caAllowed[10];
    char   caTitle[32];
    char   caIcon[128];
    char   caUrl[256];
} psMenu[512];
extern int mItem;

//用户名密码公司简称验证
int dsCltUserLogin3(caUsername, caPassword, caShortname)
{
    if(caUsername == NULL && caPassword == NULL && caShortname == NULL)
    {
        return -2;
    }
    char sql[512] = "";
    unsigned int count = 0;

    sprintf(sql, "select count(*) from dsuser where name='%s'", caUsername);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &count);

    if(count == 0)
    {
        return -1;
    }
    char caKey0[64] = "";
    char caKey[64] = "";
    count = 0;
    sprintf(caKey0, "%s%s", caUsername, caPassword);

    utMd5Ascii22(caKey0, strlen(caKey0), NULL, caKey);
    sprintf(sql, "select count(*) from dsuser where name='%s' and lkey='%s' and groupid=(select groupid from dsuser where name ='%sadmin')", caUsername, caKey, caShortname);
    printf("sql=%s\n", sql);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &count);

    if(count > 0)
    {
        return 0;
    }
    else
    {
        return -3;
    }

}

//用户名、密码验证
int dsCltUserLogin4(char * compname,  char * caUsername, char *caPassword)
{
    if((compname == NULL) ||
       (caUsername == NULL) ||
       (caPassword == NULL) ||
       (0 == strlen(compname)) ||
       (0 == strlen(caUsername)) ||
       (0 == strlen(caPassword)))
    {
        return -2;
    }

    char sql[512];
    unsigned int count = 0;

    memset(sql, 0, sizeof(sql));

    sprintf(sql, "select count(*) from dsuser where name='%s'", caUsername);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &count);

    if(count == 0)
    {
        return -1;
    }
    char caKey0[64] = "";
    char caKey[64] = "";
    count = 0;
    sprintf(caKey0, "%s%s", caUsername, caPassword);

    utMd5Ascii22(caKey0, strlen(caKey0), NULL, caKey);
    sprintf(sql, "select count(*) from dsuser where dispname='%s' and  name='%s' and lkey='%s'", compname , caUsername, caKey);
    pasDbOneRecord(sql, 0, UT_TYPE_ULONG, 4, &count);

    if(count > 0)
    {
        return 0;
    }
    else
    {
        return -3;
    }
}

/* 用户验证  */
int ncWebAuth(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char caUsername[33], caPassword[33], caIp[16];
    char caKey[64], caRight[65];
    char caModi[24];
    char caPass0[32];
    char caContuse[16];
    char *pProdSn, *pHost, *pDispName;
    char caCount[8];
    char caTemp2[1024];
    unsigned long lIp;
    unsigned long lTime;
    char caDispname[32];
    int iCount, lAuthcount;
    int iLicense = 0;
    char caTemp[1024], caAuthcount[8];
    unsigned long lUsrlevel;
    unsigned long lExpire, lUsers, lStart, lLicenseTime;
    utPltDbHead *psDbHead = utPltInitDbHead();
    long lUserid;
    char caSql[1024];
    int iReturn, lDef, lLogflag, lDefCon, lFlag;
    char caMbserno[36];
    char caPassword_t[36];
    char caScreen[12];
    long lGroupid;
    long lId;
    char caShortname[32] = "";
    strcpy(caModi, "\0");
    lTime = time(0);
    strcpy(caAuthcount, utComGetVar_sd(psShmHead, "AuthCount", "3"));
    lAuthcount = atol(caAuthcount);
    lLicenseTime = utComGetVar_ld(psShmHead, "LicenseTime", 5);
    lUserid = 0;
    char type_in[8] = "";

    utMsgPrintMsg(psMsgHead);
    // printf("pid=%d\n",getpid());
    // sleep(20);


    iReturn = utMsgGetSomeNVar(psMsgHead, 8,
                               "ModiPass",  UT_TYPE_STRING, 16, caModi,
                               "shortname",  UT_TYPE_STRING, 31, caShortname,
                               "username",  UT_TYPE_STRING, 31, caUsername,
                               "password",  UT_TYPE_STRING, 31, caPassword,
                               "clientip",  UT_TYPE_STRING, 15, caIp,
                               "contuse",   UT_TYPE_STRING, 11, caContuse,
                               "screenheight", UT_TYPE_STRING, 8, caScreen,
                               "type", UT_TYPE_STRING, sizeof(type_in), type_in);
    strcpy(caPassword_t, caPassword);
    if(strlen(caScreen) == 0) strcpy(caScreen, "768");
    utPltPutVar(psDbHead, "type", type_in);
    printf("pid=%d\n", getpid());
    //  sleep(15);
    lGroupid = 0;
    if(strlen(caUsername) > 0)
    {
        memset(caDispname, 0, sizeof(caDispname));
        sprintf(caTemp, "select id,dispname,groupid from dsuser where name='%s' ", caUsername);
        pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lUserid, UT_TYPE_STRING, 30, caDispname, UT_TYPE_LONG, 4, &lGroupid);
    }
    else
    {
        //psDbHead = utPltInitDbHead();
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/login2.htm");
        return 0;
    }
    if(strlen(caDispname) == 0) strcpy(caDispname, caUsername);
    /* 检查许可证  */
    pProdSn = utComGetVar_sd(psShmHead, "ProductSn", "\0");
    iLicense = pasInstGetLicense(pProdSn, &lExpire, &lUsers, &lStart);

    //    iLicense=1;
    if(iLicense != 1)   /* 许可证不正确  */
    {
        //psDbHead = utPltInitDbHead();
        utPltPutVarF(psDbHead, "prodsn", pProdSn);
        pHost = pasInstGetHostInfo("eth0");
        if(pHost == NULL)
        {
            utPltPutVarF(psDbHead, "message", "System Error");
            writeSysLog(psMsgHead, "01", "Fail", "License Error");
            //            ncAdmWriteLog(caUsername,caIp,"License Error");
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/login2.htm");
            return 0;
        }
        utPltPutVarF(psDbHead, "hostinfo", pHost);
        pDispName = utComGetVar_sd(psShmHead, "YourDispName", "\0");
        utPltPutVarF(psDbHead, "name", pDispName);
        writeSysLog(psMsgHead, "01", "Fail", "License Error");
        //        ncAdmWriteLog(caUsername,caIp,"License Error");
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/main_licenseerror.htm");
    }
    if(utStrIsSpaces(caContuse) && lTime > lExpire - lLicenseTime * 86400L)   /* License 即将到期  */
    {
        //psDbHead = utPltInitDbHead();
        utPltPutVarF(psDbHead, "prodsn", pProdSn);
        pHost = pasInstGetHostInfo("eth0");
        if(pHost == NULL)
        {
            utPltPutVarF(psDbHead, "message", "System Error");
            ncAdmWriteLog(caDispname, caIp, "System Error");
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/login2.htm");
            return 0;
        }
        utPltPutVarF(psDbHead, "hostinfo", pHost);
        pDispName = utComGetVar_sd(psShmHead, "YourDispName", "\0");
        utPltPutVarF(psDbHead, "name", pDispName);
        utPltPutVarF(psDbHead, "sdate", utTimFormat("%Y-%m-%d %H:%M:%S", lExpire));
        utPltPutVarF(psDbHead, "shortname", caShortname);
        utPltPutVarF(psDbHead, "username", caUsername);
        utPltPutVarF(psDbHead, "password", caPassword);
        utPltPutVarF(psDbHead, "ModiPass", caModi);
        ncAdmWriteLog(caUsername, caIp, "License Expire");
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/main_licenseexpire.htm");
        return 0;
    }
    lIp = utComHostAddress(caIp);
    if((iCount = ncAdmCheckAdmIp(psShmHead, caUsername, lIp)) > lAuthcount) /* 检查IP地址  */
    {
        //psDbHead = utPltInitDbHead();
        utPltPutVarF(psDbHead, "authcount_set", "%lu", lAuthcount);
        utPltPutVarF(psDbHead, "authcount", "%lu", iCount);
        utPltPutVarF(psDbHead, "message", ncLang("0000您已连续%lu次输入出错，终端被锁定"), iCount);
        char caMsg[128];
        sprintf(caMsg, ncLang("0000您已连续%lu次输入出错，终端被锁定"), iCount);

        writeSysLog(psMsgHead, "01", "Fail", caMsg);
        //        ncAdmWriteLog(caUsername,caIp,ncLang("0366连续三次登录不成功，?斩吮凰?));
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/login2.htm");
        return 0;
    }



    utStrDelSpaces(caUsername);
    //    utStrToLower(caUsername);
    utStrDelSpaces(caPassword);

    if((strlen(caUsername) == 0) || (strlen(caPassword) == 0))
    {
        //psDbHead = utPltInitDbHead();
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/login2.htm");
        return 0;
    }
    //printf("bbbbbbbbbbbbbbbbbb\n");    mark
    if(strcmp(utComGetVar_sd(psShmHead, "clouduser", "No"), "Yes") == 0)
    {
        iReturn = dsCltUserLogin3(caUsername, caPassword, caShortname);
    }
    else
    {
        iReturn = 0;
    }

    if(iReturn == 0)
    {
        iReturn = dsCltUserLogin(caUsername, caPassword, caIp);

    }
    if(iReturn != 0)
    {
        if(iReturn == (-1))
        {
            //psDbHead = utPltInitDbHead();
            utPltPutVarF(psDbHead, "message", ncLang("0001用户不存在"));
            sprintf(caTemp2, ncLang("0002第%d次用户不存在"), iCount);
            writeSysLog(psMsgHead, "01", "Fail", caTemp2);
            //           ncAdmWriteLog(caUsername,caIp,ncLang("0205第%d次用户不存在"),iCount);
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/login2.htm");
        }
        else if(iReturn == (-2))
        {
            //psDbHead = utPltInitDbHead();
            utPltPutVarF(psDbHead, "message", ncLang("0003口令不正确"));
            //          ncAdmWriteLog(caUsername,caIp,ncLang("0203第%d次口令不正确"),iCount);

            sprintf(caTemp2, ncLang("0004第%d次口令不正确"), iCount);
            writeSysLog(psMsgHead, "01", "Fail", caTemp2);
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/login2.htm");
        }
        else if(iReturn == (-3))
        {
            psDbHead = utPltInitDbHead();
            utPltPutVarF(psDbHead, "message", ncLang("0005公司简称不正确"));
            //          ncAdmWriteLog(caUsername,caIp,ncLang("0203第%d次口令不正确"),iCount);

            sprintf(caTemp2, ncLang("0006第%d公司简称不正确"), iCount);
            writeSysLog(psMsgHead, "01", "Fail", caTemp2);
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/login2.htm");
        }
        else
        {
            //psDbHead = utPltInitDbHead();
            //           ncAdmWriteLog(caUsername,caIp,ncLang("0204第%d次验证不正确"),iCount);
            sprintf(caTemp2, ncLang("0008第%d次验证不正确"), iCount);
            writeSysLog(psMsgHead, "01", "Fail", caTemp2);
            utPltPutVarF(psDbHead, "message", ncLang("0007验证不正确"));
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/login2.htm");
        }
        return 0;
    }
    else
    {
        //psDbHead = utPltInitDb();
        if(strcasecmp(caPassword_t, "manager") == 0)
        {
            sprintf(caTemp, "select id from dsuser where name='%s' ", caUsername);
            strcpy(caPassword_t, pProdSn);
            pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lId);
            iReturn = dsCltUserModi(lId, 2, "name", UT_TYPE_STRING, caUsername,
                                    "lkey", UT_TYPE_STRING, caPassword_t);
            sprintf(caTemp, "由于您使用了默认密码，因为安全的原因，系统自动把您的密码修改为：%s,请及时修改您的密码", caPassword_t);

            utPltPutVarF(psDbHead, "message", caTemp);


            utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/login2.htm");

            printf("aaaaaaa\n");
        }

        //   printf("aaaaaaaaaaaaaaaaaaaaaaaaa\n");
        utPltPutVarF(psDbHead, "height", "%d", atol(caScreen) - 200);
        ncAdmSetAdmIp(psShmHead, caUsername, lIp);
        //        ncAdmWriteLog(caUsername,caIp,ncLang("0173成功登录到网络督察"));

        if(strcasecmp(utComGetVar_sd(psShmHead, "secondpasswd", "No"), "YES") == 0)
        {
            utPltPutVarF(psDbHead, "username", caUsername);
            dsCltSetSessionValue(1, "mbflag", UT_TYPE_LONG, 4, 10);
            //双密码验证
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "ds/ds_secondauth.htm");
            return 0;

        }






        lDefCon = atol(utComGetVar_sd(psShmHead, "MbContDef", "0"));

        lDef = atol(utComGetVar_sd(psShmHead, "MbLoginDef", "0"));
        memset(caMbserno, 0, sizeof(caMbserno));
        sprintf(caSql, "select mbserno,logflag,flag from ncuserbindmb where userid=%d ", lUserid);
        pasDbOneRecord(caSql, 0, UT_TYPE_STRING, 16, caMbserno,
                       UT_TYPE_LONG, 4, &lLogflag,
                       UT_TYPE_LONG, 4, &lFlag);

        if(strlen(caMbserno) > 0)
        {
            if(lFlag == 0)
            {
                dsCltSetSessionValue(1, "mbflag", UT_TYPE_LONG, 4, 10);
            }
            if(lLogflag == 1)
            {
                utPltPutVar(psDbHead, "fname", "ncMbLogin");
                utPltPutVar(psDbHead, "mbserno", caMbserno);

                writeSysLog(psMsgHead, "01", "Success", ncLang("0173完成登录到系统"));

                utPltPutVarF(psDbHead, "userid", "%d", lUserid);
                utPltOutToHtml(iFd, psMsgHead, psDbHead, "ds/ds_mbauth.htm");
                return 0;
            }
        }
        else
        {
            if(lDefCon == 0)
                dsCltSetSessionValue(1, "mbflag", UT_TYPE_LONG, 4, 10);

            if(lDef == 1)
            {
                utPltPutVarF(psDbHead, "message", ncLang("1555系统要求动态验证，但该用户没有绑定移动密宝"));
                utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/login2.htm");
                return 0;
            }
        }
        writeSysLog(psMsgHead, "01", "Success", ncLang("0173完成登录到系统"));
        /*
          sprintf(caTemp,"select id,usrlevel from dsuser where name='%s' ",caUsername);
          pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lUserid,

                                UT_TYPE_LONG,4,&lUsrlevel);
         //对于日志管理员只显示日志管理界面
         if(lUsrlevel==0){
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/main_logadmin.htm");
            return 0;
         }
         else if(lUsrlevel==8){
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"vpn/main_vpn.htm");
            return 0;
         }
         else if(lUsrlevel==9){
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"vpn/main_hotel.htm");
            return 0;
         }
         else if(lUsrlevel==10){
            utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/main_2.htm");             //烟台公安
            return 0;
         }
         sprintf(caTemp,"%s/hotel/main_group.htm",utComGetVar_sd(psShmHead,"PlatePath","/home/ncmysql/nc/plate"));

         if(((lUsrlevel==3)||(lUsrlevel==2))&&(strcasecmp(utComGetVar_sd(psShmHead,"HotelVesion","No"),"Yes")==0)){
             ncsCltInfo *psServer;
             psServer = (ncsCltInfo *)utShmArray(psShmHead,NC_LNK_NCSRVINFO);
            if(psServer!=NULL){
                if(psServer->nStatus==1){
                    utPltPutVar(psDbHead,"ljstatus",ncLang("1517已连接"));
                }
                else{
                    utPltPutVar(psDbHead,"ljstatus",ncLang("1837未连接"));
                }
            }
            else
              utPltPutVar(psDbHead,"ljstatus",ncLang("1837未连接"));

        //printf("cccccccccccccccc\n");
                utPltOutToHtml(iFd,psMsgHead,psDbHead,"hotel/main_hotel.htm");
                return 0;
            }
         else if(((lUsrlevel==3)||(lUsrlevel==2))&&(utFileIsExist(caTemp)==1)){
        //printf("dddddddddd\n");
           utPltOutToHtml(iFd,psMsgHead,psDbHead,"hotel/main_group.htm");
           return 0;
          }
         else{
            char caConfFile[64];
            long lCount=0;

            sprintf(caTemp,"select count(*) from dsrole a,dsuserrole b where a.id=b.roleid and b.id=%d and (a.name='all' or a.name='全部权限') ",lUserid);
            pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
            if(lCount>0&&lGroupid==0){
        //printf("eeeeeeeeeeeeeeeeeeee\n");

                utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/main.htm");
                return 0;
            }
        //根据用户角色动态生成菜单
            else if(strcasecmp(utComGetVar_sd(psShmHead,"menubyrole","No"),"Yes")==0){

                utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/main_1.htm");
                return 0;
            }
        }

         if(lGroupid==0){
             utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/main.htm");
         }
         else{
             utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/main_group.htm");
        }

        }
        */
    }
    if(strcmp(caUsername, "admin"))
    {
        if(strlen(type_in) > 0 && !strcmp(type_in, "1"))
        {
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/download.html");
        }
        else
        {
            printf("type:%s\n", type_in);
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/main.htm");
        }
    }
    else
    {
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/nwmanage.html");
    }
    return 0;
}


//动态生成菜单脚本
int ncWebGenMenujs(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    int iReturn;
    int i, iCount, l, j, flag, m1, iNum, lId;
    char caRight[50][64];
    char caTright[64];
    pasDbCursor *psCur;
    iCount = 0;

    utPltDbHead *psDbHead;
    psDbHead = utPltInitDbHead();

    iNum = 0;
    iReturn = dsCltGetSessionValue(1, "id", UT_TYPE_LONG, sizeof(long), &lId);
    if(iReturn == 0)
    {
        //将权限放入数组 caRight中
        psCur = pasDbOpenSqlF("select a.lright from dsroleattr a,dsuserrole b where a.roleid=b.roleid and b.id=%d group by a.lright ", lId);
        if(psCur == NULL)
        {
            utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0683显示用户树"), ncLang("0554数据库出现问题，请与管理员联系"));
            return 0;
        }
        memset(caTright, 0, sizeof(caTright));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 63, caTright);

        while((iReturn == 0) && (iNum < 63))
        {
            strcpy(caRight[iNum], caTright);
            iNum++;
            memset(caTright, 0, sizeof(caTright));
            iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 63, caTright);
        }

        pasDbCloseCursor(psCur);

        //判断每个菜单项是否属于本权限

        for(i = 0; i < mItem; i++)
        {

            if(psMenu[i].iFlags == 1)
            {
                flag = 0;
                for(m1 = 0; m1 < iNum; m1++)
                {

                    if(strcmp(psMenu[i].caTitle, ncLang("1554设置(F)")) == 0)
                    {

                        if((strcmp(caRight[m1], ncLang("1538上网用户管理")) == 0) || (strcmp(caRight[m1], ncLang("1537系统设置")) == 0))
                        {
                            flag = 1;
                            break;
                        }
                    }
                    else if(strcmp(psMenu[i].caTitle, ncLang("1541实时监控")) == 0)
                    {
                        if(strcmp(caRight[m1], ncLang("1539实时监控(不包含内容)")) == 0 || strcmp(caRight[m1], ncLang("1540内容监控")) == 0 || strcmp(caRight[m1], ncLang("1541实时监控")) == 0)
                        {
                            flag = 1;

                            break;
                        }
                    }
                    else if((strcmp(psMenu[i].caTitle, ncLang("1552统计(S)")) == 0) || strcmp(psMenu[i].caTitle, ncLang("1553图表(G)")) == 0)
                    {
                        if(strcmp(caRight[m1], ncLang("1544统计图表")) == 0)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    else if(strcmp(psMenu[i].caTitle, ncLang("1543归档日志")) == 0)
                    {
                        if(strcmp(caRight[m1], ncLang("1542归档日志(不含内容)")) == 0 || strcmp(caRight[m1], ncLang("1540内容监控")) == 0 || strcmp(caRight[m1], ncLang("1543归档日志")) == 0)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    else if(strcmp(psMenu[i].caTitle, ncLang("1545规则设置")) == 0)
                    {
                        if(strcmp(caRight[m1], ncLang("1545规则设置")) == 0)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    else if(strcmp(psMenu[i].caTitle, ncLang("0191带宽管理")) == 0)
                    {
                        if(strcmp(caRight[m1], ncLang("0191带宽管理")) == 0)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    else if(strcmp(psMenu[i].caTitle, ncLang("1860客户端管理")) == 0)
                    {
                        if(strcmp(caRight[m1], ncLang("1551内网管理")) == 0)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    else if(strcmp(psMenu[i].caTitle, ncLang("0802邮件拦截")) == 0)
                    {
                        if(strcmp(caRight[m1], ncLang("0802邮件拦截")) == 0)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    else if(strcmp(psMenu[i].caTitle, ncLang("1550计费管理")) == 0)
                    {
                        if(strcmp(caRight[m1], ncLang("1550计费管理")) == 0)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    else if(strcmp(psMenu[i].caTitle, ncLang("1549系统(S)")) == 0)
                    {
                        if((strcmp(caRight[m1], ncLang("1546系统用户管理")) == 0) || (strcmp(caRight[m1], ncLang("1547查看系统日志")) == 0) || (strcmp(caRight[m1], ncLang("1548调试工具")) == 0))
                        {
                            flag = 1;
                            break;
                        }
                    }
                    else if(strcmp(psMenu[i].caTitle, ncLang("0230防火墙")) == 0)
                    {
                        if(strcmp(caRight[m1], ncLang("0230防火墙")) == 0)
                        {
                            flag = 1;
                            break;
                        }
                    }
                }

                //该菜单项有权限，输出该项
                if(flag == 1)
                {

                    iCount++;
                    utPltPutLoopVar(psDbHead, "title", iCount, psMenu[i].caTitle);
                    utPltPutLoopVarF(psDbHead, "num", iCount, "%d", iCount - 1);
                    j = i + 1;
                    l = 0;
                    while(psMenu[j].iFlags != 1 && j < mItem)
                    {
                        if(psMenu[j].iFlags == 2)
                        {
                            l = l + 25;
                        }
                        if(psMenu[j].iFlags == 4)
                        {
                            l = l + 4;
                        }
                        j++;
                    }
                    utPltPutLoopVarF(psDbHead, "hi", iCount, "%d", l);

                }

            }
        }
    }

    utPltOutToHtml(iFd, psMsgHead, psDbHead, "menu/menu.htx");
    return 0;

}


//动态生成layermenu页面
int ncWebGenlayermenu(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    int iReturn;
    int i, iCount, l, j, k;
    iCount = 0;
    char caNumitem[12];
    pasDbCursor *psCur;
    utPltDbHead *psDbHead;
    char caRight[50][64];
    char caTright[64];
    int iNum, lId, flag, m1;
    psDbHead = utPltInitDb();
    iNum = 0;
    iReturn = dsCltGetSessionValue(1, "id", UT_TYPE_LONG, sizeof(long), &lId);
    if(iReturn == 0)
    {
        psCur = pasDbOpenSqlF("select a.lright from dsroleattr a,dsuserrole b where a.roleid=b.roleid and b.id=%d group by a.lright ", lId);
        if(psCur == NULL)
        {
            utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0683显示用户树"), ncLang("0554数据库出现问题，请与管理员联系"));
            return 0;
        }
        memset(caTright, 0, sizeof(caTright));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 63, caTright);

        while((iReturn == 0) && (iNum < 63))
        {
            strcpy(caRight[iNum], caTright);
            iNum++;
            memset(caTright, 0, sizeof(caTright));
            iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 63, caTright);
        }

        pasDbCloseCursor(psCur);
    }

    iReturn = utMsgGetSomeNVar(psMsgHead, 1, "numitem", UT_TYPE_STRING, 10, caNumitem);

    k = 0;
    for(i = 0; i < mItem; i++)
    {

        if(psMenu[i].iFlags == 1)
        {
            flag = 0;
            for(m1 = 0; m1 < iNum; m1++)
            {

                if(strcmp(psMenu[i].caTitle, ncLang("1554设置(F)")) == 0)
                {
                    if((strcmp(caRight[m1], ncLang("1538上网用户管理")) == 0) || (strcmp(caRight[m1], ncLang("1537系统设置")) == 0))
                    {
                        flag = 1;
                        break;
                    }
                }
                else if(strcmp(psMenu[i].caTitle, ncLang("1541实时监控")) == 0)
                {
                    if(strcmp(caRight[m1], ncLang("1539实时监控(不包含内容)")) == 0 || strcmp(caRight[m1], ncLang("1541实时监控")) == 0)
                    {
                        flag = 1;
                        break;
                    }
                }
                else if((strcmp(psMenu[i].caTitle, ncLang("1552统计(S)")) == 0) || strcmp(psMenu[i].caTitle, ncLang("1553图表(G)")) == 0)
                {
                    if(strcmp(caRight[m1], ncLang("1544统计图表")) == 0)
                    {
                        flag = 1;
                        break;
                    }
                }
                else if(strcmp(psMenu[i].caTitle, ncLang("1543归档日志")) == 0)
                {
                    if(strcmp(caRight[m1], ncLang("1542归档日志(不含内容)")) == 0 || strcmp(caRight[m1], ncLang("1540内容监控")) == 0 || strcmp(caRight[m1], ncLang("1543归档日志")) == 0)
                    {
                        flag = 1;
                        break;
                    }
                }
                else if(strcmp(psMenu[i].caTitle, ncLang("1545规则设置")) == 0)
                {
                    if(strcmp(caRight[m1], ncLang("1545规则设置")) == 0)
                    {
                        flag = 1;
                        break;
                    }
                }
                else if(strcmp(psMenu[i].caTitle, ncLang("0191带宽管理")) == 0)
                {
                    if(strcmp(caRight[m1], ncLang("0191带宽管理")) == 0)
                    {
                        flag = 1;
                        break;
                    }
                }
                else if(strcmp(psMenu[i].caTitle, ncLang("1860客户端管理")) == 0)
                {
                    if(strcmp(caRight[m1], ncLang("1551内网管理")) == 0)
                    {
                        flag = 1;
                        break;
                    }
                }
                else if(strcmp(psMenu[i].caTitle, ncLang("0802邮件拦截")) == 0)
                {
                    if(strcmp(caRight[m1], ncLang("0802邮件拦截")) == 0)
                    {
                        flag = 1;
                        break;
                    }
                }
                else if(strcmp(psMenu[i].caTitle, ncLang("1550计费管理")) == 0)
                {
                    if(strcmp(caRight[m1], ncLang("1550计费管理")) == 0)
                    {
                        flag = 1;
                        break;
                    }
                }
                else if(strcmp(psMenu[i].caTitle, ncLang("1549系统(S)")) == 0)
                {
                    if((strcmp(caRight[m1], ncLang("1546系统用户管理")) == 0) || (strcmp(caRight[m1], ncLang("1547查看系统日志")) == 0) || (strcmp(caRight[m1], ncLang("1548调试工具")) == 0))
                    {
                        flag = 1;
                        break;
                    }
                }
                else if(strcmp(psMenu[i].caTitle, ncLang("0230防火墙")) == 0)
                {
                    if(strcmp(caRight[m1], ncLang("0230防火墙")) == 0)
                    {
                        flag = 1;
                        break;
                    }
                }
            }
            if(flag == 1)
            {
                iCount = 0;
                k++;
            }
        }

        //         printf("caTitle=%s,flag=%d,k=%d,caNumitem=%s\n",psMenu[i].caTitle,flag,k,caNumitem);
        if((psMenu[i].iFlags == 2) && (k - 1 == atol(caNumitem)) && flag == 1)
        {
            iCount++;
            if(psMenu[i].caAllowed[0] == '1')
            {
                utPltPutLoopVar2(psDbHead, "url", iCount, 1, psMenu[i].caUrl);
            }
            else
            {
                utPltPutLoopVar2(psDbHead, "url", iCount, 1, "dono()");
            }
            utPltPutLoopVar2(psDbHead, "icon", iCount, 1, psMenu[i].caIcon);
            utPltPutLoopVar2(psDbHead, "title", iCount, 1, psMenu[i].caTitle);
        }

    }
    //utPltShowDb(psDbHead);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "menu/layer.htx");
    return 0;

}




int ncWebDispModiPass(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    int iReturn;
    long lId;
    char caUsername[32];
    char caPlate[128];

    iReturn = utMsgGetSomeNVar(psMsgHead, 1, "plate",      UT_TYPE_STRING, 100, caPlate);
    iReturn = dsCltGetMyInfo(2, "Userid", &lId,
                             "Username", caUsername);
    if(iReturn < 0)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0354口令修改"),
                     ncLang("0759用户没有登陆"));
        return 0;
    }
    else
    {
        utPltDbHead *psDbHead;
        psDbHead = utPltInitDbHead();
        utPltPutVar(psDbHead, "username", caUsername);
        utPltPutVarF(psDbHead, "userid",  "%lu", lId);
        utPltPutVar(psDbHead, "plen", utComGetVar_sd(psShmHead, "plen", "8"));

        if(strlen(caPlate) > 0)
        {
            utPltOutToHtml(iFd, psMsgHead, psDbHead, caPlate);
        }
        else
        {
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/nc_modipass.htm");
        }
        return 0;
    }
}

//动态生成快捷键菜单
int ncWebGenKjcd(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    int iReturn;
    int i, iCount, l, j, flag, m1, iNum, lId;
    char caRight[50][64];
    char caTright[64];
    pasDbCursor *psCur;
    iCount = 0;

    utPltDbHead *psDbHead;
    psDbHead = utPltInitDb();

    iNum = 0;
    iReturn = dsCltGetSessionValue(1, "id", UT_TYPE_LONG, sizeof(long), &lId);
    if(iReturn == 0)
    {
        //将权限放入数组 caRight中
        psCur = pasDbOpenSqlF("select a.lright from dsroleattr a,dsuserrole b where a.roleid=b.roleid and b.id=%d group by a.lright ", lId);
        if(psCur == NULL)
        {
            utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0683显示用户树"), ncLang("0554数据库出现问题，请与管理员联系"));
            return 0;
        }
        memset(caTright, 0, sizeof(caTright));
        iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 63, caTright);

        while((iReturn == 0) && (iNum < 63))
        {
            strcpy(caRight[iNum], caTright);
            iNum++;
            memset(caTright, 0, sizeof(caTright));
            iReturn = pasDbFetchInto(psCur, UT_TYPE_STRING, 63, caTright);
        }

        pasDbCloseCursor(psCur);
    }
    //判断快捷键有没有权限
    for(m1 = 0; m1 < iNum; m1++)
    {

        if((strcmp(caRight[m1], ncLang("1537系统设置")) == 0))
        {
            utPltPutVar(psDbHead, "sz", "1");
        }
        else if(strcmp(caRight[m1], ncLang("1538上网用户管理")) == 0)
        {
            utPltPutVar(psDbHead, "swyh", "1");
        }
        else if(strcmp(caRight[m1], ncLang("1539实时监控(不包含内容)")) == 0 || strcmp(caRight[m1], ncLang("1540内容监控")) == 0 || strcmp(caRight[m1], ncLang("1541实时监控")) == 0)
        {
            utPltPutVar(psDbHead, "ssjk", "1");
        }
        else if(strcmp(caRight[m1], ncLang("1542归档日志(不含内容)")) == 0 || strcmp(caRight[m1], ncLang("1540内容监控")) == 0 || strcmp(caRight[m1], ncLang("1543归档日志")) == 0)
        {
            utPltPutVar(psDbHead, "gdrz", "1");
        }
        else if(strcmp(caRight[m1], ncLang("1544统计图表")) == 0)
        {
            utPltPutVar(psDbHead, "tj", "1");
        }
        else if(strcmp(caRight[m1], ncLang("1545规则设置")) == 0)
        {
            utPltPutVar(psDbHead, "gzsz", "1");
        }
        else if((strcmp(caRight[m1], ncLang("1546系统用户管理")) == 0) || (strcmp(caRight[m1], ncLang("1547查看系统日志")) == 0) || (strcmp(caRight[m1], ncLang("1548调试工具")) == 0))
        {
            utPltPutVar(psDbHead, "xtgl", "1");
        }


    }



    //utPltShowDb(psDbHead);
    utPltOutToHtml(iFd, psMsgHead, psDbHead, "menu/header.htx");
    return 0;

}




/* 口令修改  */
int ncWebModiPass(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{

    char caUsername[33], caPassword1[33], caPassword[33];
    char caPass0[33], caOldpass[33], caUserid[33], caIp[16];
    utPltDbHead *psDbHead;
    long lId;
    int iReturn;
    iReturn = dsCltGetMyInfo(2, "Userid", &lId,
                             "Username", caUsername);
    if(iReturn < 0)
    {
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0751用户登陆"), ncLang("0759用户没有登陆"));
        return 0;
    }

    iReturn = utMsgGetSomeNVar(psMsgHead, 3,
                               "password",     UT_TYPE_STRING, 31, caPassword,
                               "password1",    UT_TYPE_STRING, 31, caPassword1,
                               "clientip",     UT_TYPE_STRING, 15, caIp);
    utStrDelSpaces(caPassword);
    utStrDelSpaces(caPassword1);
    if(strcmp(caPassword, caPassword1) != 0)
    {
        psDbHead = utPltInitDbHead();
        utPltPutVar(psDbHead, "message", ncLang("0352口令不一致"));
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/ncmsg_back.htm");
        return 0;
    }
    iReturn = dsCltUserModi(lId, 2, "name", UT_TYPE_STRING, caUsername,
#ifdef PAS_MYSQL
                            "lkey", UT_TYPE_STRING, caPassword);
#else
                            "key", UT_TYPE_STRING, caPassword);
#endif
    if(iReturn != 0)
    {
        psDbHead = utPltInitDbHead();
        utPltPutVarF(psDbHead, "message", ncLang("0356口令修改出错, %d"), iReturn);
        //        ncAdmWriteLog(caUsername,caIp,ncLang("0709修改自己的口令出错"));
        ncsWriteSysLog(psMsgHead, caUsername, "02", "Fail", ncLang("0709修改自己的口令出错"));
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/ncmsg_back.htm");
        return 0;
    }
    else
    {
        ncsWriteSysLog(psMsgHead, caUsername, "02", "success", ncLang("0708修改自己的口令成功"));
        //        ncAdmWriteLog(caUsername,caIp,ncLang("0708修改自己的口令成功"));
        utWebDispMsg(iFd, psMsgHead, "nc/ncmsg_back.htm", ncLang("0387密码修改"),
                     ncLang("0389密码修改完成"));
    }
    return 0;
}

//第二验证登录
int ncSecondLogin(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char caUsername[33], caPassword[33], caIp[16];
    char caKey[64], caRight[65];
    char caModi[24];
    char caPass0[32];
    char caCount[8];
    unsigned long lIp;
    unsigned long lTime;
    char caMsg[256], caAuthcount[8];
    int iCount, lAuthcount;
    utPltDbHead *psDbHead;
    int iReturn;
    char caTemp[1024];
    lTime = time(0);

    iReturn = utMsgGetSomeNVar(psMsgHead, 2,
                               "username",  UT_TYPE_STRING, 31, caUsername,
                               "password2",  UT_TYPE_STRING, 31, caPassword);


    utStrDelSpaces(caUsername);

    utStrDelSpaces(caPassword);

    memset(caKey, 0, sizeof(caKey));
    sprintf(caTemp, "select key2 from dsuser where name='%s' ", caUsername);

    pasDbOneRecord(caTemp, 0, UT_TYPE_STRING, 24, caKey);
    psDbHead = utPltInitDbHead();
    if(strcmp(caPassword, caKey) == 0)
    {
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "nc/main.htm");
    }
    else
    {
        utPltPutVar(psDbHead, "username", caUsername);
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "ds/ds_secondauth.htm");
    }
    return 0;
}



int ncWebAuthShowReport(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char caUsername[33], caPassword[33], caIp[16];
    char caPlate[128];
    utPltDbHead *psDbHead;
    long lUserid;


    utMsgGetSomeNVar(psMsgHead, 4,
                     "username",  UT_TYPE_STRING, 31, caUsername,
                     "password",  UT_TYPE_STRING, 31, caPassword,
                     "clientip",  UT_TYPE_STRING, 15, caIp,
                     "plate",     UT_TYPE_STRING, 100, caPlate);

    psDbHead = utPltInitDbHead();


    utStrDelSpaces(caUsername);
    utStrDelSpaces(caPassword);

    dsCltUserLogin(caUsername, caPassword, caIp);

    utPltOutToHtml(iFd, psMsgHead, psDbHead, caPlate);
    return 0;
}

/* App用户验证  */
int ncWebAuth_app(utShmHead *psShmHead, int iFd, utMsgHead *psMsgHead)
{
    char caUsername[33], caPassword[33], caIp[16];
    char caKey[64], caRight[65];
    char caModi[24];
    char caPass0[32];
    char caContuse[16];
    char *pProdSn, *pHost, *pDispName;
    char caCount[8];
    char caTemp2[1024];
    unsigned long lIp;
    unsigned long lTime;
    char caDispname[32];
    int iCount, lAuthcount;
    int iLicense = 0;
    char caTemp[1024], caAuthcount[8];
    unsigned long lUsrlevel;
    unsigned long lExpire, lUsers, lStart, lLicenseTime;
    utPltDbHead *psDbHead = utPltInitDbHead();
    long lUserid;
    char caSql[1024];
    int iReturn, lDef, lLogflag, lDefCon, lFlag;
    char caMbserno[36];
    char caPassword_t[36];
    char caScreen[12];
    long lGroupid;
    char caSessid[68] = "";
    long lId;
    char caShortname[32] = "";
    strcpy(caModi, "\0");
    lTime = time(0);


    utMsgPrintMsg(psMsgHead);
    // printf("pid=%d\n",getpid());
    // sleep(20);


    iReturn = utMsgGetSomeNVar(psMsgHead, 4,
                               "shortname",  UT_TYPE_STRING, 31, caShortname,
                               "username",  UT_TYPE_STRING, 31, caUsername,
                               "password",  UT_TYPE_STRING, 31, caPassword,
                               "clientip",  UT_TYPE_STRING, 15, caIp);



    strcpy(caPassword_t, caPassword);


    //  sleep(15);
    lGroupid = 0;
    if(strlen(caUsername) > 0)
    {
        memset(caDispname, 0, sizeof(caDispname));
        sprintf(caTemp, "select id,dispname,groupid from dsuser where name='%s' ", caUsername);
        pasDbOneRecord(caTemp, 0, UT_TYPE_LONG, 4, &lUserid, UT_TYPE_STRING, 30, caDispname, UT_TYPE_LONG, 4, &lGroupid);
    }

    if(strlen(caDispname) == 0) strcpy(caDispname, caUsername);


    utStrDelSpaces(caUsername);
    utStrDelSpaces(caPassword);

    if((strlen(caUsername) == 0) || (strlen(caPassword) == 0))
    {
        //psDbHead = utPltInitDbHead();
        utPltPutVar(psDbHead, "buff", "{success:false,sesid:\'\',mesg:\'登录名或密码为空!\'}");
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_tmpPlate.htm");
        return 0;
    }

    iReturn = dsCltUserLoginBySid(caUsername, caPassword, caIp, caSessid);


    if(iReturn != 0)
    {
        if(iReturn == (-1))
        {
            utPltPutVar(psDbHead, "buff", "{success:false,sesid:\'\',mesg:\'用户不存在!\'}");
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_tmpPlate.htm");
            return 0;
        }
        else if(iReturn == (-2))
        {

            utPltPutVar(psDbHead, "buff", "{success:false,sesid:\'\',mesg:\'密码错误!\'}");
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_tmpPlate.htm");
            return 0;
        }
        else
        {
            utPltPutVar(psDbHead, "buff", "{success:false,sesid:\'\',mesg:\'验证错误!\'}");
            utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_tmpPlate.htm");
            return 0;
        }

    }
    else
    {
        utPltPutVarF(psDbHead, "buff", "{success:true,sesid:\'%s\',mesg:\'验证完成!\'}", caSessid);
        utPltOutToHtml(iFd, psMsgHead, psDbHead, "v8/smm/lan_tmpPlate.htm");

    }



    writeSysLog(psMsgHead, "01", "Success", ncLang("0173完成登录到系统"));

    return 0;
}