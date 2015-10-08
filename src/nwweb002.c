
int nwWebCtl_net(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caMessage[256];
    char caMod[16],caDel[16],caAdd[16],caApp[16],caCurPg[16];
    char caSum[16],caGtype[16],caFlags[16],caId[16],caRnum[16],caService[16];
    char caSid[16],caLimctl[16],caDateid[16],caEtime[16],caStime[16];
    char caLip[16],caUip[16],caLport[16],caUport[16],caProt[12],caListid[16],caUrl[64];
    char *pBuf,*pSelect,*p;
    unsigned long lLip;
    long  lStart,lStartRec,lCurPg,lTotPg;
    long  lRnum,lSum,lGtype,lFlags,lId,lDateid,lLimctl,lStime,lEtime,lSid,lService;
    long  lLport,lUport,lProt,lCount,lCount1,lRowNum;
    int   iReturn,i;
    utPltDbHead *psDbHead;

#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif
	iReturn = utMsgGetSomeNVar(psMsgHead,21,
	                "sum",     UT_TYPE_STRING, 12, caSum,    /* 记录总数                      */
	                "gtype",   UT_TYPE_STRING, 12, caGtype,  /* 1-User 2-Group 3-All          */
	                "id",      UT_TYPE_STRING, 12, caId,      /*    限制对象ID                */
                    "rnum",    UT_TYPE_STRING, 12, caRnum,    /* 序号                         */
	                "add",     UT_TYPE_STRING, 12, caAdd,
	                "mod",     UT_TYPE_STRING, 12, caMod,
	                "del",     UT_TYPE_STRING, 12, caDel,
	                "sid",     UT_TYPE_STRING, 12, caSid,
	                "ctl",     UT_TYPE_STRING, 12, caCtl,
	                "dateid",  UT_TYPE_STRING, 12, caDateid,
	                "etime",   UT_TYPE_STRING, 8,  caEtime,
	                "stime",   UT_TYPE_STRING, 8,  caStime,
	                "sip",     UT_TYPE_STRING, 15, caSip,
	                "eip",     UT_TYPE_STRING, 15, caEip,
	                "ssport",  UT_TYPE_STRING, 5,  caSsport,
	                "seport",  UT_TYPE_STRING, 5,  caSeport,
	                "dsport",  UT_TYPE_STRING, 5,  caDsport,
	                "deport",  UT_TYPE_STRING, 5,  caDeport,
	                "prot",    UT_TYPE_STRING, 2,  caProt,
	                "app",     UT_TYPE_STRING,12,  caApp,
	                "CurPg",   UT_TYPE_STRING,12,  caCurPg);
	lCurPg = atol(caCurPg);
	if(lCurPg == 0) lCurPg = 1;
	lRowNum = 24;
	lStart = (lCurPg - 1) * lRowNum;          
	lRnum = atol(caRnum);                
    lSum = atol(caSum);
    lGtype = atol(caGtype);
    lId = atol(caId);
    lDateid = atol(caDateid);
    lCtype  = atol(caCtl);
    lSsport = atol(caSsport);
    lSeport = atol(caSeport);
    lDsport = atol(caDsport);
    lDeport = atol(caDeport);
    lProt = atol(caProt);
    lDtype = 3;
    
    if(!utStrIsSpaces(caAdd)) {  /* 增加记录  */
        if(!utStrIsSpaces(caSip) && !utStrIsValidIp(caSip)) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0433起始IP地址为空或格式不对,正确的格式应为 xxx.xxx.xxx.xxx"));
            return 0;
        }
        if(utStrIsSpaces(caEip)) {
            strcpy(caEip,caSip);
        }
        if(!utStrIsValidIp(caUip)) {
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0864终止IP地址格式不对,正确的格式应为 xxx.xxx.xxx.xxx"));
            return 0;
        }
        lStime = ncUtlIsValidTime(caStime);
        lEtime = ncUtlIsValidTime(caEtime);
        if(lStime < 0 || lEtime < 0) {
             utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0245访问限制"),ncLang("0435起始或终止时间不正确"));
             return 0;
        }
        lSip = ntohl(pasIpcvtLong(caSip));
        lEip = ntohl(pasIpcvtLong(caEip));
         
        iReturn = pasDbExecSqlF("insert into nwctlnet(sid,gtype,uid,num,dtype,sip,eip,ctype,dateid,stime,etime,prot,dsport,deport,ssport,seport,mtime) \
                                  values (%lu,%d,%lu,%d,%d,%lu,%lu,%d,%lu,%lu,%lu,%d,%d,%d,%d,%d,%lu)",
                           pasGetSid("nwctlnet","sid"),lGtype,lId,lRnum,lDtype,lSip,lDip,lCtype,lDateid,lStime,lEtime,lProt,lDsport,lDeport,lSsport,lSeport,time(0));
        if(iReturn != 0) {
            sprintf(caMessage,"insert into nwctlnet(sid,gtype,uid,num,dtype,sip,eip,ctype,dateid,stime,etime,prot,dsport,deport,ssport,seport,mtime) \
                                  values (0,%d,%lu,%d,%d,%lu,%lu,%d,%lu,%lu,%lu,%d,%d,%d,%d,%d,%lu)",
                          lGtype,lId,lRnum,lDtype,lSip,lDip,lCtype,lDateid,lStime,lEtime,lProt,lDsport,lDeport,lSsport,lSeport,time(0));
            printf(" Sql=%s\n",caMessage);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0314记录增加出错, 可能为记录重复"));
            pasDbRollback(NULL);
            return 0;
        }
        else {
            pasDbCommit(NULL);
            break;
        }
    }
    else if(!utStrIsSpaces(caDel)) { /* 删除记录  */
        char caVar[16];
   	    for(i=0;i<lRowNum;i++) {
   	        sprintf(caVar,"ch%d",i+1);
   	        p = utMsgGetVar_s(psMsgHead,caVar);
   	        if(p) {
                iReturn = pasDbExecSqlF("delete from nwctlnet where sid = %s",p);
                if(iReturn == 0) {
                    pasDbCommit(NULL);
                }
   	        }
   	    }
    }
    else if(!utStrIsSpaces(caMod)) { /* 修改记录  */
        lSid = atol(caSid);
        iReturn = pasDbExecSqlF("update nwctlnet set gtype=%d,uid=%lu,num=%lu,dtype=%d,sip=%lu,eip=%lu,ctype=%d,dateid=%lu,stime=%lu,etime=%lu,prot=%d,\
                dsport=%lu,deport=%lu,ssport=%lu,seport=%lu,mtime=%lu where sid = %lu",
                lGtype,lId,lRnum,lDtype,lSip,lDip,lCtype,lDateid,lStime,lEtime,lProt,lDsport,lDeport,lSsport,lSeport,time(0),lSid);
        if(iReturn != 0) {
            sprintf(caMessage,"update nwctlnet set gtype=%d,uid=%lu,num=%lu,dtype=%d,sip=%lu,eip=%lu,ctype=%d,dateid=%lu,stime=%lu,etime=%lu,prot=%d,\
                dsport=%lu,deport=%lu,ssport=%lu,seport=%lu,mtime=%lu where sid = %lu",
                lGtype,lId,lRnum,lDtype,lSip,lDip,lCtype,lDateid,lStime,lEtime,lProt,lDsport,lDeport,lSsport,lSeport,time(0),lSid);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0308记录修改出错, 可能为记录重复 %s"),pasDbErrorMsg(NULL));
            pasDbRollback(NULL);
            return 0;
        }
        else {
            pasDbCommit(NULL);
            break;
        }
    }
    else if(!utStrIsSpaces(caApp)) { /* 将所有的控制时间均置为0  */

        return 0;    
    }
    
    psDbHead = utPltInitDb();
    
    lCount = 0;

    iReturn = pasDbOneRecord("select count(*) from nwctlnet where gtype=:gtype and id=:id and dtype = 3",2,
                 "gtype",UT_TYPE_LONG,lGtype,
                 "id",UT_TYPE_LONG,lId,
                 UT_TYPE_LONG,4, &lSumRec);
    if(iReturn != 0) {
        lSumRec = 0;
    }

    psCur = pasDbOpenSqlF("select sid,num,sip,dispname,userid from ncuser %s order by userid limit %lu,%lu",caWhere,lStartRec,lRowNum);
    if(psCur == NULL) {
        printf("Sql:[select username,dispname,userid from ncuser %s order by userid limit %lu,%lu]\n",caWhere,lStartRec,lRowNum);
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0683显示用户树"),"Data Base Error, %s ",pasDbErrorMsg(NULL));
        return 0;
    }
    psDbHead = utPltInitDb();
    iReturn = pasDbFetchInto(psCur,UT_TYPE_STRING,31,caUsername,
                                   UT_TYPE_STRING,31,caDispname,
                                   UT_TYPE_LONG,4,&lUserid);



        lCount = ncWebLimShowIpLim(psDbHead,lGtype,lId,lFlags,1,lStart,lRowNum);
    }
    if(lCount < 0) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0068Ip地址列表"),ncLang("0548数据库查询出错"));
        return 0;
    }
    pBuf = pasUtlCvtSqlSelectA("select name,id from nclimdatedes order by name",ncLang("0384每天,0;"));
    if(pBuf) {
        pSelect= pasUtlCvtInputSelect("dateid",caDateid,pBuf);
        free(pBuf);
    }
    utPltPutVar(psDbHead,"dateid", pSelect);
    free(pSelect);
    if(lFlags == NC_LIMIT_SERVICE) {
        pBuf = pasUtlCvtSqlSelect("select distinct name,id from ncipindex");
        if(pBuf) {
            pSelect= pasUtlCvtInputSelect("ser1",caService,pBuf);
            free(pBuf);
        }
        utPltPutVar(psDbHead,"service", pSelect);
        free(pSelect);
    }
    else if(lFlags == NC_LIMIT_IPADDRESS) {
        pSelect= pasUtlCvtInputSelect("prot",caProt,ncLang("0453全部,0;TCP,6;UDP,17;"));
        utPltPutVar(psDbHead,"prot", pSelect);
        free(pSelect);
        utPltPutVarF(psDbHead,"lport", "%lu",0);
        utPltPutVarF(psDbHead,"uport", "%lu",65535L);
        utPltPutVarF(psDbHead,"lip", "%s","0.0.0.0");
        utPltPutVarF(psDbHead,"uip", "%s","255.255.255.255");
    }
    else if(lFlags == NC_LIMIT_IPLIST) {
        pBuf = pasUtlCvtSqlSelect("select name,pid from ncippoltype");
        if(pBuf) {
            pSelect= pasUtlCvtInputSelect("listid",caListid,pBuf);
            free(pBuf);
        }
        utPltPutVar(psDbHead,"listid", pSelect);
        free(pSelect);
    }
    else if(lFlags == NC_LIMIT_WEBLIST) {
        pBuf = pasUtlCvtSqlSelect("select name,pid from ncwebpolclass ");
        if(pBuf) {
            pSelect= pasUtlCvtInputSelect("listid",caListid,pBuf);
            free(pBuf);
        }
        utPltPutVar(psDbHead,"listid", pSelect);
        free(pSelect);
    }
    utPltPutVar(psDbHead,"stime","00:00:00");
    utPltPutVar(psDbHead,"etime","23:59:59");
    utPltPutVarF(psDbHead,"flags","%d",lFlags);
    utPltPutVarF(psDbHead,"gtype","%lu",lGtype);
    utPltPutVarF(psDbHead,"id","%lu",lId);
    utPltPutVar(psDbHead,"appname",ncLimGetAppName(lGtype,lId));
    utPltPutVarF(psDbHead,"sum","%lu",lSum);
    utPltPutVarF(psDbHead,"rnum", "%04d",lCount+1);
    utPltPutVarF(psDbHead,"CurPg", "%d",lCurPg);
    if(lSum > 0) {
        lTotPg = (lSum - 1) / lRowNum + 1;
    }
    else {
        lTotPg = 0;
    }
    utPltPutVarF(psDbHead,"TotPg", "%d",lTotPg);
    utPltPutVarF(psDbHead,"TotRec", "%d",lSum);


    i = 0;
    while(lCount < lRowNum) {
        i++;
        lCount++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%d",lCount);
    }
    
    if(lFlags == NC_LIMIT_INTERNET) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_out.htm");
    }
    else if(lFlags == NC_LIMIT_SERVICE) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_service.htm");
    }   
    else if(lFlags == NC_LIMIT_IPADDRESS) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_ip.htm");
    }
    else if(lFlags == NC_LIMIT_IPLIST) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_iplist.htm");
    }      
    else if(lFlags == NC_LIMIT_WEBHOST) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_web.htm");
    }  
    else if(lFlags == NC_LIMIT_WEBLIST) {
        utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_lim_weblist.htm");
    }
    return 0;
}
