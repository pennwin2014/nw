/******************************************************************
 * File Name: 
 *		SafeEngine.h
 * Description: 
 *		Head file for Safe Engine.
 *		Definition of data structure, function
 * Create Date:
 *		8/17/1999
 * Last Modified:
 *		6/13/2000		Add SEH_  functions.
 *
 ******************************************************************/
#ifndef _SAFEENGINE_H
#define _SAFEENGINE_H

#define MAX_X500_NAME_LEN              256
#define MAX_SERIAL_LEN                 128
#define MAX_ALGO_PARA_LEN              1024
#define MAX_UNIQUE_ID_LEN              128
#define MAX_PUBLIC_KEY_LEN             1024
#define MAX_SIGNATURE_LEN              512

#define MAX_UNKNOWN_EXT                8
#define MAX_OID_SIZE                   32 
#define MAX_EXT_VAL_SIZE               1024

/* Error code definition */
#define	SE_SUCCESS                         0L
#define	SE_ERROR_LOAD_LIBRARY              0x82040000
#define	SE_ERROR_MEMORY                    0x82040001
#define	SE_ERROR_PRIVATE_KEY_DEV           0x82040002
#define	SE_ERROR_PRIVATE_KEY_PASSWORD      0x82040003
#define	SE_ERROR_CERT_CHAIN_DEV            0x82040004
#define	SE_ERROR_CERT_CHAIN_PASSWORD       0x82040005
#define	SE_ERROR_CERT_DEV                  0x82040006
#define	SE_ERROR_CERT_PASSWORD             0x82040007
#define	SE_ERROR_PRIVATE_KEY_TIME_OUT      0x82040008
#define	SE_ERROR_NO_ENOUGH_BUFFER          0x82040009
#define	SE_ERROR_INITIAL_SESSION           0x8204000A
#define	SE_ERROR_CLEAR_SESSION             0x8204000B
#define	SE_ERROR_SIGN                      0x8204000C
#define	SE_ERROR_VERIFY_SIGN               0x8204000D
#define	SE_ERROR_DIGEST                    0x8204000E
#define	SE_ERROR_INVALID_CERT              0x8204000F
#define	SE_ERROR_ENVELOPE                  0x82040010
#define	SE_ERROR_GET_CERT_FROM_LDAP        0x82040011
#define	SE_ERROR_CERT_EXPIRED              0x82040012
#define	SE_ERROR_GET_CERT_CHAIN            0x82040013
#define	SE_ERROR_INVALID_CERT_CHAIN        0x82040014
#define	SE_ERROR_VERIFY_CERT_CHAIN         0x82040015
#define	SE_ERROR_CERT_REVOKED              0x82040016
#define	SE_ERROR_INVALID_CRL               0x82040017
#define	SE_ERROR_CONNECT_OCSP              0x82040018
#define SE_ERROR_OCSP_REQ_ENCODE           0x82040019
#define	SE_ERROR_OCSP_RES                  0x8204001A
#define	SE_ERROR_INVALID_OCSP_RES          0x8204001B
#define	SE_ERROR_OCSP_RES_EXPIRED          0x8204001C
#define	SE_ERROR_OCSP_RES_VERIFY_SIGN      0x8204001D
#define	SE_ERROR_CERT_STATUS_UNKNOWN       0x8204001E
#define	SE_ERROR_SYN_CRYPT                 0x8204001F
#define	SE_ERROR_GET_CERT_INFO             0x82040020
#define	SE_ERROR_GET_CERT_DETAIL           0x82040021
#define	SE_ERROR_GET_CERT_UNIQUE_ID        0x82040022
#define	SE_ERROR_GET_CERT_EXTENSION        0x82040023
#define	SE_ERROR_PEM_ENCODE                0x82040024
#define	SE_ERROR_PEM_DECODE                0x82040025
#define	SE_ERROR_GEN_RANDOM                0x82040026
#define	SE_ERROR_INVALID_P12_PARAM         0x82040027
#define	SE_ERROR_INVALID_PRIVATE_KEY       0x82040028
#define	SE_ERROR_KEY_PAIR                  0x82040029
#define	SE_ERROR_P12_ENCODE                0x8204002A
#define	SE_ERROR_INVALID_P12               0x8204002B
#define	SE_ERROR_P12_DECODE                0x8204002C
#define	SE_ERROR_ASYN_CRYPT                0x8204002D
#define	SE_ERROR_INVALID_OID               0x8204002E
#define	SE_ERROR_INVALID_LDAP_ADDRESS      0x8204002F
#define SE_ERROR_LDAP_ADDRESS              0x82040030
#define SE_ERROR_LDAP_CONNECT              0x82040031
#define SE_ERROR_LDAP_BIND                 0x82040032
#define SE_ERROR_OID_NOT_FOUND             0x82040033
#define SE_ERROR_GET_CERT_CLASS            0x82040034
#define SE_ERROR_READ_CONFIG               0x82040035
#define SE_ERROR_PRIKEY_NOT_LOAD           0x82040036
#define SE_ERROR_PRIVATE_KEY_PASSWORD_LEN  0x82040037
#define	SE_ERROR_API_EXPIRED               0x820400FF

/* data structure definition */
typedef unsigned short	uint16;
typedef unsigned long   uint32;
typedef unsigned char	uint8;

typedef struct SERIAL_INFO_{
    uint8              serialNumber[MAX_SERIAL_LEN];
    uint16             serialNumberLen;
} SERIAL_INFO;

typedef struct ALGORITHM_INFO_{
    uint16             algorithm;
    uint8              parameters[MAX_PUBLIC_KEY_LEN];
    uint16             parametersLen;
} ALGORITHM_INFO;

typedef  struct X500_NAME_INFO_{
    char               countryName[MAX_X500_NAME_LEN];
    char               organizationName[MAX_X500_NAME_LEN];
    char               organizationalUnitname[MAX_X500_NAME_LEN];
    char               stateOrProvinceName[MAX_X500_NAME_LEN];
    char               commonName[MAX_X500_NAME_LEN];
    char               localityName[MAX_X500_NAME_LEN];
    char               title[MAX_X500_NAME_LEN];
    char               surname[MAX_X500_NAME_LEN];
    char               givenName[MAX_X500_NAME_LEN];
    char               initials[MAX_X500_NAME_LEN];
    char               email[MAX_X500_NAME_LEN];
    char               postalAddress[MAX_X500_NAME_LEN];
    char               postalCode[MAX_X500_NAME_LEN];
    char               postalOfficeBox[MAX_X500_NAME_LEN];  /*” ’˛–≈œ‰∫≈*/
    char               telephoneNumber[MAX_X500_NAME_LEN];
    char               telexNumber[MAX_X500_NAME_LEN];      
} X500_NAME_INFO;


typedef struct PUBLIC_KEY_INFO_{
    uint8              derPublicKey[MAX_ALGO_PARA_LEN];
    uint16             derPublicKeyLen;
    ALGORITHM_INFO     algorithmInfo;
} PUBLIC_KEY_INFO;

typedef struct UNIQUE_ID_INFO_{
    uint8              uniqueID[MAX_UNIQUE_ID_LEN];
    uint16             uniqueIDLen;
} UNIQUE_ID_INFO;

typedef struct CA_SIGNATURE_INFO_{
	uint8              caSignature[MAX_SIGNATURE_LEN];
	uint16             caSignatureLen;
} CA_SIGNATURE_INFO;

typedef struct{
    X500_NAME_INFO             directoryName;
    SERIAL_INFO                serialNumber;
} USER_AuthKeyIdentifier_INFO;

typedef struct{
    char        notBefore[64];   /*YYYYMMDDHHMMSS*/
    char        notAfter[64];    /*YYYYMMDDHHMMSS*/
} USER_PrivateKeyUsagePeriod_INFO;

typedef struct{
	uint8       policyIdentifier[4][32];
	uint32      policyIdentifierSize[4];
	char        CPS_url[4][MAX_X500_NAME_LEN];
} USER_CertificatePolicies_INFO;

typedef struct{
    uint8       cA;
    uint32      pathLenConstraint;
} USER_BasicConstraints_INFO;

typedef struct{
    uint32      keyUsage_bit;
} USER_KeyUsage_INFO;

typedef struct{
    uint32      neCertType_bit;
} USER_neCertType_INFO;

typedef struct{
    uint32      setCertType_bit;
} USER_setCertType_INFO;

typedef struct{
    uint8       hashedRootKey[64];
	uint32      hashedRootKeySize;
} USER_setHashedRootKey_INFO;

typedef struct{
	uint8       oid[MAX_UNKNOWN_EXT][MAX_OID_SIZE];
	uint32      oidSize[MAX_UNKNOWN_EXT];
	uint8       critical[MAX_UNKNOWN_EXT];
	uint8       value[MAX_UNKNOWN_EXT][MAX_EXT_VAL_SIZE]; 
	uint32      valueSize[MAX_UNKNOWN_EXT];
	uint32      extSize; 
} USER_privateExt_INFO;

typedef struct{
    USER_AuthKeyIdentifier_INFO       AuthKeyIdentifier;
    USER_CertificatePolicies_INFO     CertificatePolicies;
    USER_PrivateKeyUsagePeriod_INFO   PrivateKeyUsagePeriod;
    USER_BasicConstraints_INFO        BasicConstraints;
    USER_KeyUsage_INFO                KeyUsage;
    USER_neCertType_INFO              neCertType;
    USER_setCertType_INFO             setCertType;
    USER_setHashedRootKey_INFO        setHashedRootKey;
	uint32                            selfCertClass;
	char                              revocationURL[MAX_X500_NAME_LEN];
    USER_privateExt_INFO              privateExt;
} USER_EXTENSION_INFO;

typedef struct{
    uint8                 version;
    SERIAL_INFO           serialNumber;
    ALGORITHM_INFO        signature;
    X500_NAME_INFO        issuer;
    char                  notBefore[64];   /*YYYYMMDDHHMMSS*/
    char                  notAfter[64];    /*YYYYMMDDHHMMSS*/
    X500_NAME_INFO        subject;
    PUBLIC_KEY_INFO       subjectPublicKeyInfo;
    UNIQUE_ID_INFO        issuerUniqueID; 
    UNIQUE_ID_INFO        subjectUniqueID;
	CA_SIGNATURE_INFO     caSignature;

	uint32                extensionsFlag;
	uint32                criticalFlag;

    USER_EXTENSION_INFO   extensions;

} CERT_INFO;

typedef struct{
	unsigned short ReaderID;
	char ReaderDesc[128];
} READER_INFO;

typedef struct{
	char devDesc[128];
	int bAlgorithm;
	int bSaveCertEnable;
	int iSupportReaderNum;
	READER_INFO Reader[10];
	char CSPLib[128];
	char CardLib[128];
	char STDLib[128];
	char INTLib[128];
	char ProvName[128];
} DEVICE_INFO;

typedef void* HSE;

/* Function definition */
#ifdef __cplusplus
extern "C" {
#endif                                                                        

long SE_InitialSession(unsigned long privatekeydevicetype,  char *privatekeydeviceparameter, char *privatekeypassword,  unsigned long privatekeytimeout,  unsigned long certchaindevicetype,  char *certchaindeviceparameter,  char *certchainpassword );
long SE_InitialSessionEx(unsigned char * privatekey, unsigned short privatekeylen, char *privatekeypassword, unsigned char *rootcert, unsigned short rootcertlen);
long SE_ClearSession();

long SE_SignData(unsigned char *origindata, unsigned long length, unsigned short signmethod, unsigned char *signeddata, unsigned long *signedlength);
long SE_Digest(unsigned char *data, unsigned long length, unsigned short method, unsigned char *digest, unsigned long  *digestlength);
long SE_VerifySignData(unsigned char *origindata, unsigned long originlength, unsigned short signmethod, unsigned char *signeddata, unsigned long signedlength, unsigned char *certificate, unsigned short certlength);
long SE_Envelope(unsigned short envolpetype, unsigned char *indata, unsigned long inlength, unsigned char *outdata, unsigned long *outlength, unsigned char *certificate, unsigned short certlength);
long SE_EnvelopeEx(unsigned short envolpetype, unsigned char *indata, unsigned long inlength, unsigned char *outdata, unsigned long *outlength, unsigned char *certificate, unsigned short certlength, int AlgID);

long SE_GetSelfCertificate(unsigned long devicetype, char *deviceparameter, char *password, unsigned char *certificate, unsigned short *certificatelength);
long SE_VerifyCertificate(unsigned char *certificate, unsigned short length);
long SE_GetCertificate(char *SerialNo, char *URL, unsigned char *certificate, unsigned short *certlen);
long SE_VerifyCertificateOnline(unsigned char *certificate, unsigned short length);
long SE_ChangePassword(unsigned long device, char *deviceparameter, char *oldpassword, char *newpassword);

long SE_GetCertInfo(unsigned char *DerCert, unsigned short DerCertLen, CERT_INFO *pCertInfo);
long SE_GetCertDetail(unsigned char *DerCert, unsigned short DerCertLen, unsigned short ItemNo, unsigned char *ItemValue, unsigned short *ItemLength);

long SE_PEMEncode(unsigned char *indata, unsigned long indatalen, unsigned char *pemdata, unsigned long *pemlen);
long SE_PEMDecode(unsigned char *pemdata, unsigned long pemlen, unsigned char *outdata, unsigned long *outlen);

long SE_GenRandomBytes(unsigned char *randombytes, int randombyteslen);
long SE_EncryptData(unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *sdbikey);
long SE_DecryptData(unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *sdbikey);
long SE_EncryptDataEx(unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *key, int keylen, int AlgID);
long SE_DecryptDataEx(unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *key, int keylen, int AlgID);

long SE_SetConfiguration(long mode);

long SE_EncodePKCS12(unsigned char * pcDERCert,
                               unsigned long   lCertSize,
                               char          * pcPassword,
                               unsigned char * pcP12Buffer,
                               unsigned long * plP12Size);

long SE_GetCertUniqueID(unsigned char *cert, unsigned short certlen, char *uniqueid, unsigned short *idlen);
long SE_GetCertInfoByOID(unsigned char *cert, unsigned short certlen, char *oid, char *info, unsigned short *infolen);

long SE_ShowVersion(long *plModualNo, long *plVersion);

long SE_GetCertValidDate(unsigned char *cert, unsigned short certlen, long *datanum);
long SE_GetCertClass(unsigned char *cert, unsigned short certlen, unsigned long *lCertClass);
long SE_GetCertUsage(unsigned char *cert, unsigned short certlen, unsigned long *lKeyUsage);

long SE_GetDeviceList(int *totalDevNum, unsigned long *devType);
long SE_GetDeviceInfo(unsigned long devType, DEVICE_INFO *pdevInfo);
long SE_GetDeviceType(unsigned short ReaderID, unsigned short CardType, unsigned long *devType);

/* Functions support multi-threads */
long SEH_InitialSession(HSE *hSE, unsigned long privatekeydevicetype,  char *privatekeydeviceparameter, char *privatekeypassword,  unsigned long privatekeytimeout,  unsigned long certchaindevicetype,  char *certchaindeviceparameter,  char *certchainpassword );
long SEH_InitialSessionEx(HSE *hSE, unsigned char * privatekey, unsigned short privatekeylen, char *privatekeypassword, unsigned char *rootcert, unsigned short rootcertlen);
long SEH_ClearSession(HSE hSE);

long SEH_SignData(HSE hSE, unsigned char *origindata, unsigned long length, unsigned short signmethod, unsigned char *signeddata, unsigned long *signedlength);
long SEH_Digest(HSE hSE, unsigned char *data, unsigned long length, unsigned short method, unsigned char *digest, unsigned long  *digestlength);
long SEH_VerifySignData(HSE hSE, unsigned char *origindata, unsigned long originlength, unsigned short signmethod, unsigned char *signeddata, unsigned long signedlength, unsigned char *certificate, unsigned short certlength);
long SEH_Envelope(HSE hSE, unsigned short envolpetype, unsigned char *indata, unsigned long inlength, unsigned char *outdata, unsigned long *outlength, unsigned char *certificate, unsigned short certlength);
long SEH_EnvelopeEx(HSE hSE, unsigned short envolpetype, unsigned char *indata, unsigned long inlength, unsigned char *outdata, unsigned long *outlength, unsigned char *certificate, unsigned short certlength, int AlgID);

long SEH_GetSelfCertificate(HSE hSE, unsigned long devicetype, char *deviceparameter, char *password, unsigned char *certificate, unsigned short *certificatelength);
long SEH_VerifyCertificate(HSE hSE, unsigned char *certificate, unsigned short length);
long SEH_GetCertificate(HSE hSE, char *SerialNo, char *URL, unsigned char *certificate, unsigned short *certlen);
long SEH_VerifyCertificateOnline(HSE hSE, unsigned char *certificate, unsigned short length);
long SEH_ChangePassword(HSE hSE, unsigned long device, char *deviceparameter, char *oldpassword, char *newpassword);

long SEH_GetCertInfo(HSE hSE, unsigned char *DerCert, unsigned short DerCertLen, CERT_INFO *pCertInfo);
long SEH_GetCertDetail(HSE hSE, unsigned char *DerCert, unsigned short DerCertLen, unsigned short ItemNo, unsigned char *ItemValue, unsigned short *ItemLength);

long SEH_PEMEncode(HSE hSE, unsigned char *indata, unsigned long indatalen, unsigned char *pemdata, unsigned long *pemlen);
long SEH_PEMDecode(HSE hSE, unsigned char *pemdata, unsigned long pemlen, unsigned char *outdata, unsigned long *outlen);

long SEH_GenRandomBytes(HSE hSE, unsigned char *randombytes, int randombyteslen);
long SEH_EncryptData(HSE hSE, unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *sdbikey);
long SEH_DecryptData(HSE hSE, unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *sdbikey);
long SEH_EncryptDataEx(HSE hSE, unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *key, int keylen, int AlgID);
long SEH_DecryptDataEx(HSE hSE, unsigned char *indata, int indatalen, unsigned char *outdata, int *outdatalen, unsigned char *key, int keylen, int AlgID);

long SEH_SetConfiguration(HSE hSE, long mode);

long SEH_EncodePKCS12(HSE hSE,
                                unsigned char * pcDERCert,
                                unsigned long   lCertSize,
                                char          * pcPassword,
                                unsigned char * pcP12Buffer,
                                unsigned long * plP12Size);

long SEH_GetCertUniqueID(HSE hSE, unsigned char *cert, unsigned short certlen, char*uniqueid, unsigned short *idlen);
long SEH_GetCertInfoByOID(HSE hSE, unsigned char *cert, unsigned short certlen, char *oid, char *info, unsigned short *infolen);

long SEH_ShowVersion(HSE hSE, long *plModualNo, long *plVersion);

long SEH_GetCertValidDate(HSE hSE, unsigned char *cert, unsigned short certlen, long *datanum);
long SEH_GetCertClass(HSE hSE, unsigned char *cert, unsigned short certlen, unsigned long *lCertClass);
long SEH_GetCertUsage(HSE hSE, unsigned char *cert, unsigned short certlen, unsigned long *lKeyUsage);

long SEH_GetDeviceList(HSE hSE, int *totalDevNum, unsigned long *devType);
long SEH_GetDeviceInfo(HSE hSE, unsigned long devType, DEVICE_INFO *pdevInfo);
long SEH_GetDeviceType(HSE hSE, unsigned short ReaderID, unsigned short CardType, unsigned long *devType);

#ifdef __cplusplus
}
#endif                                                                        

#endif	/* _SAFEENGINE_H */
