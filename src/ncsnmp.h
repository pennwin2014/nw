/* Created By Liyunming at 2008/09/05
*/
#ifndef __NCSNMPDEF___
#define __NCPKGDEF___
typedef struct ncSnmpSwitch_s {
    char        caName[32];        /* ����  */
    char        caIp[32];          /* ��ַ                      */
    unsigned short nPort;          /* �˿�                      */
    unsigned short nMax;           /* ���MAC��ַ��             */
    char        caLogin[32];               /* ��¼��                    */
    char        caKey[32];                /* Key                       */
    char        caType[32];                /* ��ʽ: SNMP,Telnet,Http    */
    unsigned long lLastTime;       /* ����ѯʱ��              */
    unsigned long lTimeStep;       /* ��ѯʱ����              */
} ncSnmpSwitch;

#endif
