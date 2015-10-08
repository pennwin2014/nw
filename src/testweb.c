#include <stdio.h>
#include <string.h>
main()
{
    char caIn[256];
    char caSub[256];
    char *p;
    printf(" Content:");
    gets(caIn);
    while(1) {
        printf("Sub:");
        gets(caSub);
        p= ncWebSubStr(caIn,strlen(caIn),caSub);
        if(p) {
            printf(" Yes it is [%s]\n",p);
        }
        else {
            printf(" No match \n");
        }
    }
}
