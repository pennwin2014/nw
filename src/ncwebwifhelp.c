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
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>

#include "utoall.h"
#include "utoplt01.h"
#include "pasdb.h"
#include "ncdef.h"
#include"dlfcn.h"

//判断验证页面的目录
static char *ncGetIdtype(int lType)
{
  static char caType[256];
  memset(caType,0,sizeof(caType));
  switch(lType){
  case 11:
  	strcpy(caType,"身份证");
  	break;
  case 90:
  	strcpy(caType,"军官证");
  	break;
  case 15:
  	strcpy(caType,"学生证");
  	break;
  case 91:
  	strcpy(caType,"警官证");
  	break;
  case 92:
  	strcpy(caType,"士兵证");
  	break;
  case 93:
  	strcpy(caType,"户口薄");
  	break;
  case 94:
  	strcpy(caType,"护照");
  	break;
  case 95:
  	strcpy(caType,"台胞证");
  	break;
  case 96:
    strcpy(caType,"同乡证");
    break;
  case 99:
  	strcpy(caType,"其它证件");
  	break;
  default :
  	strcpy(caType,"其它证件");
  	break;
  	
  		
  }
  
    	          return &caType[0];
}

/* 密保生成动态KEY */
int ncMBAuthGenpass(utShmHead *psShmHead,char *caPass)
{
	int iReturn;
	int iRet;
	pasDbCursor *psCur;
	char caSerno[36],caTemp[2048];
	char cbcode[1024],caMbkey[1024];
	void *libc;
    int (*Passpod_SysInit)();
	int (*Passpod_athn)(char *cbdata, char *pwd, char *cbcode, int codesize, int md5, int drift);
	void (*Passpod_SysClose)();
	int (*Passpod_sync)(char *cbdata, char *pwd, char *cbcode, int codesize, int md5, int drift);
	libc=dlopen("libpasspod.so", RTLD_LAZY);
	Passpod_SysInit = dlsym(libc, "Passpod_SysInit");
	Passpod_athn = dlsym(libc, "Passpod_athn");
	Passpod_SysClose = dlsym(libc, "Passpod_SysClose");
	Passpod_sync = dlsym(libc, "Passpod_sync"); 
	iReturn=0;
	iRet=0;
	iReturn = Passpod_SysInit();
	if(iReturn==1){
	sprintf(caTemp,"select mbserno,mbkey from ncmbserno ");
	 psCur = pasDbOpenSql(caTemp,0);
   if(psCur == NULL) {
        return 0;
   }
   iRet = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSerno,
                                       UT_TYPE_STRING,1020,caMbkey);
    while(iRet==0){
    	
    	iReturn = Passpod_athn(caMbkey, caPass, cbcode, sizeof(cbcode), 0, 300);
			if(iReturn==1000){
				break;
			}
    	
    	memset(caSerno,0,sizeof(caSerno));
    	memset(caMbkey,0,sizeof(caMbkey));
    	iRet = pasDbFetchInto(psCur,UT_TYPE_STRING,15,caSerno,
                                       UT_TYPE_STRING,1020,caMbkey);
    }
    pasDbCloseCursor(psCur);
    
  }
    printf("iReturn=%d\n",iReturn);
    iRet=0;
	if(iReturn==1000){

	  		pasDbExecSqlF("update ncmbserno set mbkey='%s' where mbserno='%s' ",cbcode,caSerno);
	  		iRet=1;
	  
		}

	 Passpod_SysClose();
    return iRet;
}



/* 显示协助登记用户界面  */
int ncWebShowHelpUser(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    char caZcrx[20];
    int iNum,iReturn;
    char caMsg[128];
    char caUsragent[256];
    char caEng_flag[16];
    long lFlag,lLogin;
    char caPath_all[256];
    utPltDbHead *psDbHead;
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);
#endif

 utMsgGetSomeNVar(psMsgHead,4,
				"zcrx",    UT_TYPE_STRING, 2, caZcrx,
				"msg",     UT_TYPE_STRING,100,caMsg,
				"eng_flag",    UT_TYPE_STRING,6,caEng_flag,
				"usragent",    UT_TYPE_STRING,255,caUsragent);
				
if(strlen(caZcrx)==0){
	 if((strcasecmp(utComGetVar_sd(psShmHead,"StartSysJGUser","No"),"Yes")==0)||(strcasecmp(utComGetVar_sd(psShmHead,"HotelAuth","No"),"Yes")==0)||strcasecmp(utComGetVar_sd(psShmHead,"AuthByCif","No"),"center")==0||(strcasecmp(utComGetVar_sd(psShmHead,"StartXMZAJK","No"),"Yes")==0)){
			strcpy(caZcrx,"1");
   }
   else{
   	strcpy(caZcrx,"0");
  }
}

	//判断是否手机还是PC
	  lLogin=0;
    lLogin=ncIsMobile(caUsragent);
/*	
		lFlag=0;
    lLogin=0;
    utStrToLower(caUsragent);
			//对于不是MOZILLA浏览器，调用手机登录界面
		    if(strstr(caUsragent,"mozilla")&&strlen(caUsragent)>0){
		    	lFlag=1;	    	
		    }
		    else if(strlen(caUsragent)>0){
		    	lLogin=1;
		    }
		    else{
		    	lLogin=0;
		    }
		    
		    if(lFlag>0){
		    	if(strstr(caUsragent,"android"))
		    	  lLogin=1;
		     if(strstr(caUsragent,"apple"))
		    	  lLogin=1;
		     if(strstr(caUsragent,"iphone"))
		    	  lLogin=1;
		     if(strstr(caUsragent,"windows ce"))
		    	  lLogin=1;
		      if(strstr(caUsragent,"blackberry"))
		    	  lLogin=1;
		    	if(strstr(caUsragent,"symbian"))
		    	  lLogin=1;
		    }

*/

    psDbHead = utPltInitDb();
    utPltPutVar(psDbHead,"stime",utTimFormat("%Y-%m-%d",time(0)));
    if(atol(caZcrx)==0){
    utPltPutVar(psDbHead,"djcheck","checked");
  }
  else{
  	utPltPutVar(psDbHead,"zjcheck","checked");
  }
  utPltPutVar(psDbHead,"msg",caMsg);
   if(atol(caZcrx)==0){  
   	      sprintf(caMsg,"nc%s/nc_helpuserzc.htm",ncGetAuthPath_2(psShmHead,lLogin,atol(caEng_flag)));
 //         printf("caPlate=%s\n",caMsg);
          sprintf(caPath_all,"/home/ncmysql/nc/plate/%s",caMsg);
          if(!utFileIsExist(caPath_all)){
          	strcpy(caMsg,"nc/nc_helpuserzc.htm");
          }
//           printf("caPlate=%s\n",caMsg);
   	
  /* 	
   	  if(lLogin==1){                               //手机
   	  	 if(atol(caEng_flag)==1){
   	  	 	 utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_helpuserzc_ensj.htm");         //手机英文登记牌
   	  	}
   	  	else{
   	  		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_helpuserzc_sj.htm");         //手机中文登记牌
   	  	}
   	  }
   	  else{
   	  	if(atol(caEng_flag)==1){
   	  		 utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_helpuserzc_en.htm");               //pc英文登记牌
   	  	}
   	  	else{
   	  		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_helpuserzc.htm");               //pc中文登记牌
    	   
    	  }
    	}
 */
    }
    else{
    	   	sprintf(caMsg,"nc%s/nc_helpuserzc_zj.htm",ncGetAuthPath_2(psShmHead,lLogin,atol(caEng_flag)));
          printf("caPlate=%s\n",caMsg);
          sprintf(caPath_all,"/home/ncmysql/nc/plate/%s",caMsg);
          if(!utFileIsExist(caPath_all)){
          	strcpy(caMsg,"nc/nc_helpuserzc_zj.htm");
          }
    	
/*    	if(lLogin==1){
    		if(atol(caEng_flag)==1){
    			utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_helpuserzc_zj_ensj.htm");        //证件英文手机
    		} 
    		else{
    			utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_helpuserzc_zj_sj.htm");           //证件中文手机
    		}
    	}
    	else{
    		if(atol(caEng_flag)==1){
    			utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_helpuserzc_enzj.htm");            //证件英文PC
    		}
    		else{
    			utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_helpuserzc_zj.htm");               //证件中文pc
    		}
    	}
    	*/
    	
    }
    utPltOutToHtml(iFd,psMsgHead,psDbHead,caMsg);               //pc中文登记牌

    return 0;
}    




//添加注册用户
int ncHelpUserLogin(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iReturn;
    char caDispname[32],caStime[16],caIdfytype[16],caIdno[32],caPasswd[32],caCount[32],caSeat[32],caZcrx[1],caFlyno[32];
    char caMsg[128];
    char caUsername[36];
    char caPassword1[36],caPassword[36];
    long lUserid;
    char caTemp[1024];
    long lTime;
    ncUserInfo *psUserInfo=NULL;
   	ncUserCont *psUser;
    char caMessage[256];
    char caIp[20];
    utPltDbHead *psDbHead;
    unsigned long lIp;
    
    char caUsragent[256];
    char caEng_flag[16];
    long lFlag,lLogin;
    char caPath_all[256];
    
#ifdef LDEBUG
    utMsgPrintMsg(psMsgHead);    
#endif
utMsgPrintMsg(psMsgHead);    
   
   	iReturn = utMsgGetSomeNVar(psMsgHead,13,
   	      "dispname",      UT_TYPE_STRING, 30, caDispname,
   	      "stime",    UT_TYPE_STRING, 10,caStime,
   	      "idfytype",    UT_TYPE_STRING, 4,caIdfytype,
   	      "idno",    UT_TYPE_STRING, 20,caIdno,
   	      "passwd",   UT_TYPE_STRING, 20,caPasswd,
   	      "count",      UT_TYPE_STRING,16,caCount,
   	      "seat",     UT_TYPE_STRING,16,caSeat,
   	      "zcrx",     UT_TYPE_STRING,1,caZcrx,
   	      "flyno",    UT_TYPE_STRING,30,caFlyno,
   	      "zcrx",     UT_TYPE_STRING,1,caZcrx,
   	      "clientip",    UT_TYPE_STRING, 19, caIp,
   	      "eng_flag",    UT_TYPE_STRING,6,caEng_flag,
				 "usragent",    UT_TYPE_STRING,255,caUsragent);
				
		//判断是否手机还是PC
	  lLogin=0;
    lLogin=ncIsMobile(caUsragent);			
				
								
				
		lIp = ntohl(pasIpcvtLong(caIp));			
    memset(caMsg,0,sizeof(caMsg));
   
    if(utStrIsSpaces(caDispname)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","增加用户","姓名不能为空");
        return 0;
    }
    if(utStrIsSpaces(caPasswd)) {
        utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm","请输入用移动密保生成的KEY","请输入移动密保生成KEY");
        return 0;
    }
 
 strcpy(caUsername,utTimFormat("%y%m%d%H%M%S",time(0)));
// printf("caUsername=%s\n",caUsername);
   
   iReturn=ncMBAuthGenpass(psShmHead,caPasswd);
    if(iReturn<1){
    	if(atol(caEng_flag)==1){ 
    	strcpy(caMsg,"Invalid key,please try again.");
    }
    else{
    	strcpy(caMsg,"无效的Key,请重试!");
    }

    }
    else{
 //验证通过 
   if(strlen(caStime)>0){  
    lTime=utTimStrToLong("%Y-%m-%d",caStime);	
  }
  else{
  	lTime=time(0);
  }
    sprintf(caTemp,"insert into nchelpuser(username,dispname,passwd,flyno,seat,count,idtype,idno,stime,addtime,flag) \
      values('%s','%s','%s','%s','%s','%s','%s','%s',%d,%d,%d)",caUsername,caDispname,caPasswd,caFlyno,caSeat,caCount,caIdfytype,caIdno,lTime,time(0),atol(caZcrx));
      printf("caTemp=%s\n",caTemp);
      iReturn=pasDbExecSql(caTemp,0);
//   	pasDbExecSqlF("insert into nchtlpuser(username,dispname,passwd,flyno,seat,count,idtype,idno,stime,addtime,flag),
//      values('%s','%s','%s','%s','%s','%s','%s','%s',%d,%d,%d)",caUsername,caDispname,caPasswd,caFlyno,caSeat,caCount,caIdfytype,caIdno,lTime,time(0),atol(caZcrx));
      //将用户信息插入到用户管理中
      if(iReturn==0){

          iReturn=pasDbExecSqlF("insert into ncmobuser(username,password) values('%s','%s')",caUsername,caPasswd);

           sprintf(caPassword1,"%s%s",caUsername,caPasswd);
          utMd5Ascii22(caPassword1,strlen(caPassword1),NULL,caPassword);
          lUserid =  pasGetSid("ncuser","userid");
//          printf("insert into ncuser (userid,username,dispname,compname,groupid,ip,mac,addtime,lasttime,useflags,password,email,position,idfytype,idfyno,pointphyname,idfyunit,country,localflag,expiretime,usestatus,sex,userlevel,vlanid) values \
//                                                         (%lu,'%s','%s','',0,0,'',%lu,%lu, %d,'%s','%s','%s','%s','%s','%s','%s','%s',%lu,%lu,1,'%s',%d,%d)\n",
//                                                         lUserid,caUsername,caDispname,time(0),time(0),0,caPassword,"","",caIdfytype,caIdno,"","",caCount,0,time(0)+3600*6,"",0,0);
                                                         
                                                         
          iReturn = pasDbExecSqlF("insert into ncuser (userid,username,dispname,compname,groupid,ip,mac,addtime,lasttime,useflags,password,email,position,idfytype,idfyno,pointphyname,idfyunit,country,localflag,expiretime,usestatus,sex,userlevel,vlanid) values \
                                                         (%lu,'%s','%s','',0,0,'',%lu,%lu, %d,'%s','%s','%s','%s','%s','%s','%s','%s',%lu,%lu,1,'%s',%d,%d)",
                                                         lUserid,caUsername,caDispname,time(0),time(0),0,caPassword,"","",caIdfytype,caIdno,"","",caCount,0,time(0)+3600*6,"",0,0);

           	if(iReturn==0){
           		ncWebModiUserFromBuf(psShmHead,lUserid);
           		//验证登录
           		  psUser = ncUtlGetUserContByName(psShmHead,caUsername);
           		  if(psUser){
           		  	psUserInfo = ncUtlDoLogin(psShmHead,psUser,NC_LOGIN_AUTH,lIp,NULL,caMessage);
           		  	  if(psUserInfo == NULL) {        	
                		utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0778用户验证"),caMessage);
                		return 0;
            				}
            				if(strcasecmp(utComGetVar_sd(psShmHead,"AuthSendHtml","No"),"No")==0) {
                    	ncCtlIptDelRedict(lIp);
                		}
                		psDbHead = utPltInitDb();
                		char caPlatePath[128],caDip[20],caPort[12];
                		    ncUtlGetAuthIp(psShmHead,lIp,caDip,caPort,caPlatePath);
                	  utPltPutVar(psDbHead,"ip",caDip);
                    utPltPutVar(psDbHead,"port",caPort);
                		
                		
                		if(strcasecmp(utComGetVar_sd(psShmHead,"BeatHeat","NO"),"YES") == 0){
						  				utPltPutVar(psDbHead,"beat_flag","1");
						  				long lBeattime=1000/3*utComGetVar_ld(psShmHead,"beattimeout",3600);
						  				utPltPutVarF(psDbHead,"beattime","%d",lBeattime);
						  			}
						  			utPltPutVar(psDbHead,"password",caPasswd);
						  			utPltPutVar(psDbHead,"account",caUsername);
						  			utPltPutVarF(psDbHead,"sid","%d",psUserInfo->lId);
/*
						  			if(lLogin==1){                              //手机
						  				if(atol(caEng_flag)==1){                  //手机英文
						  					utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_helpuserzc_ok_ensj.htm");
						  				}
						  				else{                                     //手机中文
						  					 utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_helpuserzc_ok_sj.htm");
						  				}
						  			}
						  			else{
						  				if(atol(caEng_flag)==1){                 //pc英文
						  					 utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_helpuserzc_ok_en.htm");
						  				}
						  				else{				                              //pc中文		  			
                     		utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_helpuserzc_ok.htm");
                      }
                    }
*/

          sprintf(caMsg,"%s%s/nc_helpuserzc_ok.htm",caPlatePath,ncGetAuthPath_2(psShmHead,lLogin,atol(caEng_flag)));
  //        printf("caPlate=%s\n",caMsg);
          sprintf(caPath_all,"/home/ncmysql/nc/plate/%s",caMsg);
          if(!utFileIsExist(caPath_all)){
          	strcpy(caMsg,"nc/nc_helpuserzc_ok.htm");
          }
//           printf("caPlate=%s\n",caMsg);
           utPltOutToHtml(iFd,psMsgHead,psDbHead,caMsg);
                		 return 0;
                		
                		
           		  }
             }
           }
          
          
          
          
    
    }
   

    utMsgAdd_s(&psMsgHead,"msg",caMsg);
    ncWebShowHelpUser(psShmHead, iFd, psMsgHead);
    return 0;
}


//酒店宾馆集中验证
int ncsReqAuthCenter(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    ncHoteluser stHoteluser;
    char caUsername[64],caPassword[64];
    long iReturn,lStatus;
    char caPass_t[64];
    char caBuf[1024];
    char caNcname[32];
    char caMsg[64];
    char caSlgbm[16];
    long iRet;
    long iAuth=0;
    char caPass_temp[36],caUsername_temp[36];
    char caPassword0[36],caKey[36];
    char caDispname[32],caPosition[32],caIdfytype[8],caIdfyno[36],caFamaddr[150],caCountry[8],caSex[8];
    lStatus=-1;
//   utMsgPrintMsg(psMsgHead);

		 utMsgGetSomeNVar(psMsgHead,3, "username",UT_TYPE_STRING,32,caUsername,
		                                         "password",UT_TYPE_STRING,32,caPassword,
		                                         "ncname",UT_TYPE_STRING,20,caNcname);
	
	
	 if(strlen(caUsername)==0||strlen(caPassword)==0){
 	
 	     pasTcpResponse(iFd,psMsgHead,
                    NULL,                /* 密钥，暂时不用  */
                    1,
                    "status",UT_TYPE_LONG,-1);
     return 0;
 	
}
	
sprintf(caBuf,"select password,dispname,position,idfytype,idfyno,'',country,sex from ncuser where username='%s' and usestatus=1 and useflags!=1 ",caUsername);
	


       
       printf("caBuf=%s\n",caBuf);
       iAuth=0;
        memset(caPass_t,0,sizeof(caPass_t));
        iRet=pasDbOneRecord(caBuf,0,UT_TYPE_STRING,32,caPass_t,
                                    UT_TYPE_STRING,31,caDispname,
                                    UT_TYPE_STRING,31,caPosition,
                                    UT_TYPE_STRING,3,caIdfytype,
                                    UT_TYPE_STRING,18,caIdfyno,
                                    UT_TYPE_STRING,148,caFamaddr,
                                    UT_TYPE_STRING,3,caCountry,
                                    UT_TYPE_STRING,4,caSex);
    
       if(iRet==0){
       	  sprintf(caPassword0,"%s%s",caUsername,caPassword);
       	  utMd5Ascii22(caPassword0,strlen(caPassword0),NULL,caKey);
       	  if(strcmp(caPass_t,caKey) == 0) { /* 通过验证  */
            	iAuth=1;
           } 
           else{
           	 strcpy(caPass_temp,utStrLower(caPassword));
           	 strcpy(caUsername_temp,utStrLower(caPassword));
           	 sprintf(caPassword0,"%s%s",caUsername_temp,caPass_temp);
		       	  utMd5Ascii22(caPassword0,strlen(caPassword0),NULL,caKey);
		       	  if(strcmp(caPass_t,caKey) == 0) { /* 通过验证  */
		            	iAuth=1;
		           } 
		           else{
		           	 strcpy(caPass_temp,utStrUpper(caPassword));
		           	 strcpy(caUsername_temp,utStrUpper(caPassword));
		           	 sprintf(caPassword0,"%s%s",caUsername_temp,caPass_temp);
				       	  utMd5Ascii22(caPassword0,strlen(caPassword0),NULL,caKey);
				       	  if(strcmp(caPass_t,caKey) == 0) { /* 通过验证  */
				            	iAuth=1;
				           }
				           else{
				           	iAuth=0;
				          } 
		          }
           }
//       	 	sprintf(caUsername_t,"%s%s",caUsername,utStrLower(caPassword));
         
          
         
       	  
       	  
      }
        if((iAuth==1)&&iRet==0){
        	 lStatus=0;
        	 strcpy(stHoteluser.caDispname,caDispname);
        	 strcpy(stHoteluser.caPosition,caPosition);
        	 strcpy(stHoteluser.caIdfytype,caIdfytype);
        	 strcpy(stHoteluser.caIdfyno,caIdfyno);
        	 strcpy(stHoteluser.caFamaddr,caFamaddr);
        	 strcpy(stHoteluser.caCountry,caCountry);
        	 strcpy(stHoteluser.caSex,caSex);
        	}
        else{
        	 lStatus=-1;
        	}
        
        pasTcpResponse(iFd,psMsgHead,
                    NULL,                /* 密钥，暂时不用  */
                    2,
                    "status",UT_TYPE_LONG,lStatus,
                    "data",UT_TYPE_STRUCT,&stHoteluser,sizeof(ncHoteluser));
                    
          
    return 0;
}



 //帐号自助信息
int ncWebHelpUserList(utShmHead *psShmHead, int iFd,utMsgHead *psMsgHead)
{
    int iNum,iReturn;
    utPltDbHead *psDbHead;
    char *p;
    char caVar[32];
    int i=0;
   	
   	char caPlate[128];
   	char caSid[16],caAdd[16],caDel[16];
	char caModi[16];
	char caTotPg[16],caCurPg[16];
	long lRowNum,lStartRec,lCurPg;
	long lCount,lTid,lTotPg;
	char caTemp[1024];
  char caCond[512];
  long lSid,lNum;
  char caSear[36];
  char caUsername[24],caDispname[32],caPasswd[24],caFlyno[32],caSeat[32],caCount[24],caIdtype[16],caIdno[36];
  long lAddtime,lFlag;
    pasDbCursor *psCur;
    char caVlanid[16],caPosition[36];

	iReturn = utMsgGetSomeNVar(psMsgHead,3,
	                "TotPg",      UT_TYPE_STRING, 12, caTotPg,
			            "CurPg",      UT_TYPE_STRING, 12, caCurPg,
			            "sear",      UT_TYPE_STRING,30,caSear);
  
  
 // printf("cheID=%s\n",caCheID); 	             
	                
   lCurPg = atol(caCurPg); 
    if(lCurPg <= 0) lCurPg = 1;


    
      lRowNum = 23;
      lStartRec = (lCurPg - 1) * lRowNum;    
      psDbHead = utPltInitDb();
      utPltPutVar(psDbHead,"sear",caSear);
     memset(caCond,0,sizeof(caCond));
     if(strlen(caSear)>0){
     	  sprintf(caCond+strlen(caCond)," and (username like '%c%s%c' or dispname like '%c%s%c' or flyno like '%c%s%c' or idno like '%c%s%c' )",'%',caSear,'%','%',caSear,'%','%',caSear,'%','%',caSear,'%');

      }
     
      sprintf(caTemp,"select count(*) from nchelpuser where 1=1 ");
      strcat(caTemp,caCond);
      
      iReturn=pasDbOneRecord(caTemp,0,UT_TYPE_LONG,4,&lCount);
        if(iReturn != 0) {
            utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0213定义邮件分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
      sprintf(caTemp,"select sid,username,dispname,passwd,flyno,seat,count,idtype,idno,addtime,flag from nchelpuser where 1=1 ");
      strcat(caTemp,caCond);
      sprintf(caTemp+strlen(caTemp)," order by sid desc  limit %d,%d ",lStartRec,lRowNum);
      
      
      psCur=pasDbOpenSql(caTemp,0);
      if(psCur==NULL){
      	    utPltFreeDb(psDbHead);
            utWebDispMsg(iFd,psMsgHead,"nc/ncmsg_back.htm",ncLang("0212定义表单分类"),ncLang("0174出错%s"),pasDbErrorMsg(NULL));
            return 0;
        }
			lSid=0;
		  memset(caUsername,0,sizeof(caUsername));
		  memset(caDispname,0,sizeof(caDispname));
		  memset(caPasswd,0,sizeof(caPasswd));
		  memset(caFlyno,0,sizeof(caFlyno));
		  memset(caSeat,0,sizeof(caSeat));
		  memset(caCount,0,sizeof(caCount));
		  memset(caIdtype,0,sizeof(caIdtype));
		  memset(caIdno,0,sizeof(caIdno));
		  lAddtime=0;
		  lFlag=0;		
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                   UT_TYPE_STRING,20,caUsername,
                                   UT_TYPE_STRING,30,caDispname,
                                   UT_TYPE_STRING,20,caPasswd,
                                   UT_TYPE_STRING,30,caFlyno,
                                   UT_TYPE_STRING,30,caSeat,
                                   UT_TYPE_STRING,20,caCount,
                                   UT_TYPE_STRING,10,caIdtype,
                                   UT_TYPE_STRING,30,caIdno,
                                   UT_TYPE_LONG,4,&lAddtime,
                                   UT_TYPE_LONG,4,&lFlag);
                                  
                              
   		 
      iNum=0;
      while(iReturn==0||iReturn==1405){
      	iNum++;
      	utPltPutLoopVarF(psDbHead,"sid",iNum,"%lu",lSid);
	      utPltPutLoopVarF(psDbHead,"iNum",iNum,"%lu",iNum);	
        utPltPutLoopVar(psDbHead,"username",iNum,caUsername);
        utPltPutLoopVar(psDbHead,"dispname",iNum,caDispname);
        utPltPutLoopVar(psDbHead,"passwd",iNum,caPasswd);
        utPltPutLoopVar(psDbHead,"flyno",iNum,caFlyno);
        utPltPutLoopVar(psDbHead,"seat",iNum,caSeat);
        utPltPutLoopVar(psDbHead,"count",iNum,caCount);
        utPltPutLoopVar(psDbHead,"idtype",iNum,ncGetIdtype(atol(caIdtype)));
        utPltPutLoopVar(psDbHead,"idno",iNum,caIdno);
        utPltPutLoopVar(psDbHead,"addtime",iNum,utTimFormat("%m-%d %H:%M:%S",lAddtime));
        if(lFlag==0){
        	utPltPutLoopVar(psDbHead,"flag_desc",iNum,"登记牌");
        }
        else{
        	utPltPutLoopVar(psDbHead,"flag_desc",iNum,"证件");
        }
  
       
      
			lSid=0;
		  memset(caUsername,0,sizeof(caUsername));
		  memset(caDispname,0,sizeof(caDispname));
		  memset(caPasswd,0,sizeof(caPasswd));
		  memset(caFlyno,0,sizeof(caFlyno));
		  memset(caSeat,0,sizeof(caSeat));
		  memset(caCount,0,sizeof(caCount));
		  memset(caIdtype,0,sizeof(caIdtype));
		  memset(caIdno,0,sizeof(caIdno));
		  lAddtime=0;
		  lFlag=0;		
      iReturn=pasDbFetchInto(psCur,UT_TYPE_LONG,4,&lSid,
                                   UT_TYPE_STRING,20,caUsername,
                                   UT_TYPE_STRING,30,caDispname,
                                   UT_TYPE_STRING,20,caPasswd,
                                   UT_TYPE_STRING,30,caFlyno,
                                   UT_TYPE_STRING,30,caSeat,
                                   UT_TYPE_STRING,20,caCount,
                                   UT_TYPE_STRING,10,caIdtype,
                                   UT_TYPE_STRING,30,caIdno,
                                   UT_TYPE_LONG,4,&lAddtime,
                                   UT_TYPE_LONG,4,&lFlag);
  				}
       lTotPg = (lCount - 1) / lRowNum + 1;
       pasDbCloseCursor(psCur);
       utPltPutVarF(psDbHead,"TotPg", "%lu", lTotPg);
       utPltPutVarF(psDbHead,"CurPg", "%lu", lCurPg);
       utPltPutVarF(psDbHead,"TotRec", "%lu", lCount);
 //  printf("end\n"); 
 
   i=0;
    while(iNum < lRowNum) {
        i++;
        utPltPutLoopVarF(psDbHead,"snum", i,"%ld",iNum);
        iNum++;
    }
 
   			 
      utPltOutToHtml(iFd,psMsgHead,psDbHead,"nc/nc_list_helpuser.htm");
    return 0;
}