#include <stdio.h>  
#include <errno.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/stat.h> 

static unsigned int total = 0; 

int sum(const char *fpath, const struct stat *sb, int typeflag) 
{ 

     total += sb->st_size;  
     return 0; 

 }  
 

unsigned int getDirSize(char* dirPath){
	if (!dirPath || access(dirPath, R_OK)) { 

         return 1; 

     } 

     if (ftw(dirPath, &sum, 1)) { 

         perror("ftw"); 

         return 2; 

     } 

    
	return total;
}
 
int main(int argc, char **argv) 
 { 
    total = 0;
	total = getDirSize(argv[1]);
	printf("%u\n", total); 
     return 0; 

 }