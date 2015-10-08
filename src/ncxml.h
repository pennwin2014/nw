#ifndef __NCXMLDEF__
#define __NCXMLDEF__

#define NC_XMLTYPE_ARRAY    1
#define NC_XMLTYPE_STRING   2

typedef struct ncXmlVarValue_s {
    int  iType;
    char *pVar;
    char *pValue;
} ncXmlVarValue;


#endif