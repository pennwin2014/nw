#ifndef __HASH_API_H
#define __HASH_API_H

typedef struct ncProIptIp_s {
    unsigned long lSip;
    unsigned long lDip;
    unsigned short nSport;
    unsigned short nDport;
    union 
    {
	unsigned long lFlags;
	struct Actions_s
	{
		unsigned long cPort:8;	
		unsigned long opts:3;	
		unsigned long timeout:5; /* no use */
		unsigned long qosid:16;	/* QOSID */
	} a;
    } b;
} ncProIptIp;

/* the apis */
int (*ncProIptGetSysInfo) (unsigned int *iVersion, unsigned int *iBufSize, unsigned int *iUseRate);
int (*ncProIptBufInit) (unsigned int iUseRate, unsigned int iDefault, unsigned int ID);
int (*ncProIptSetIpAcl) (ncProIptIp *psIp);
int (*ncProIptGetIpAcl) (ncProIptIp *psIp);
int (*ncProIptDelIpAcl) (ncProIptIp *psIp);
int (*ncProIptDoIpAcl) (char *pHead, 
        int(*fDoFun)(char *pHead, void *baseAddr, const unsigned long id1, const unsigned long id2));

/* simple internal func */
int (*UserHashDel) (const unsigned long id);
int (*UserClliDel) (const unsigned long id);


/* For debug */
void (*UserPrintMesg) (FILE *fd);
void (*UserPrintStruct) (FILE *fd);
void (*UserPrintHashElem) (FILE *fd, const unsigned long id);
void (*UserPrintClliElem) (FILE *fd, const unsigned long index);

#endif /*__HASH_API_H*/
