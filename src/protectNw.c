//�ػ����̱��رպ���������
#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
//�ļ��ߴ�
#define FILE_LENGTH 100 
//�������� 
#define TARGET_NAME "nw" 
//���ҽ����г������� 
#define NAME_FIND "nw" 
//���Ŀ¼ 
#define DIR_OUT_FILE "/home/ncmysql/nw/bin/out" 

int daemon(int nochdir,int noclose)   
{   
    pid_t pid;   
	//��init���̳�Ϊ�²������̵ĸ����� 
    pid = fork();   
	//�����������ʧ�� 
	if (pid < 0)   
    {   
        perror("fork");   
		return -1;   
    }   
	//�������˳����� 
	if (pid != 0)   
    {   
        exit(0);   
    }   
	//�����µĻỰ 
    pid = setsid();   
	if (pid < -1)   
    {   
        perror("set sid");   
		return -1;   
    }   
	//���ĵ�ǰ����Ŀ¼,������Ŀ¼�޸ĳɸ�Ŀ¼ 
	if (!nochdir)   
    {   
        chdir("/");   
    }   
	//�ر��ļ������������ض����׼���룬����ϴ������ 
	//����׼��������ض��򵽿��豸 
	if (!noclose)   
    {   
		int fd;   
        fd = open("/dev/null",O_RDWR,0);   
		if (fd != -1)   
        {   
            dup2(fd,STDIN_FILENO);   
            dup2(fd,STDOUT_FILENO);   
            dup2(fd,STDERR_FILENO);   
			if (fd > 2)   
            {   
                close(fd);   
            }   
        }   
    }   
	//�����ػ����̵��ļ�Ȩ�޴������� 
    umask(0);
	//umask(0027);   
	return 0;   
}   

//�Ƿ���ƥ����ַ������򷵻�1��û�з���0 
//src:Դ�ַ��� 
//dst��Ŀ���ַ��� 
//len:Դ�ַ������Ƚϵĳ��� 
int match(char *src,char *dst,int len)   
{   
	int i = 0;   
	int j = 0;   
	int size_dst = 0;   

	//���Ŀ���ַ����ĳ��� 
    size_dst = strlen(dst);   
	//���Ŀ���ַ����ĳ��ȴ���len������ʧ�� 
	if (size_dst > len)   
    {   
		return 0;   
    }      
	//��ʼ�Ƚ� 
	for (i = 0;i < len;i++)   
    {   
		for (j = 0;j < size_dst;j++)   
        {   
			if (src[i + j] != dst[j])   
            {   
				break;   
            }   
        }   
		if (j == size_dst)   
        {   
			return 1;   
        }   
    }   
	return 0;   
}   

int openNw(){
	system("chmod 777 /home/ncmysql/nw/bin/restart");
	system("/home/ncmysql/nw/bin/restart");
	return 0;
}

int main(int argc,char *argv[])   
{   
	int fd = 0;   
	char buf[FILE_LENGTH];   
	//�����ػ����� 
    daemon(0,0);   

	while (1)   
    {   
		//����־ 
        openlog(argv[0],LOG_CONS|LOG_PID,LOG_USER);   
		//�鿴�����Ƿ����� 
		//�½�����ļ� 
        system("touch "DIR_OUT_FILE);   
		//��ó���ID 
		//system("ps -e|grep "NAME_FIND" >> "DIR_OUT_FILE);   
		system("ps -e|grep -v nw_c | grep "NAME_FIND" >> "DIR_OUT_FILE);  

		//������ļ� 
        fd = open(DIR_OUT_FILE,O_CREAT|O_RDONLY,0755);   
		//��ջ��� 
        memset(buf,0,FILE_LENGTH);   
		//��ȡȫ�� 
        read(fd,buf,FILE_LENGTH);  
		
		//�ж��Ƿ��г����ļ����� 
		if (match(buf,TARGET_NAME,FILE_LENGTH-1))   
        {   
            syslog(LOG_INFO,"nw is running");  
        }   
		else 
        {   
            syslog(LOG_INFO,"nw is not running");
			//���г��� 
			openNw();
        }
		//���� 
        sleep(5);   
		//ɾ������ļ� 
        system("rm "DIR_OUT_FILE);   
		//���� 
        sleep(5);   
    }   
	//�ر���־ 
    closelog();   
	return 0;   
}  
