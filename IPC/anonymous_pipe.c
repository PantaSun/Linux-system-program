/**
 * 匿名管道简单使用
 * 匿名管道特点：是一种半双工的通信方式，只能在具有父子关系的进程间使用
 * PantaSun
 * 2019-09-10
 */


#include <unistd.h>
#include <stdio.h>
#include "tlpi_hdr.h"
#include <string.h>
#include <sys/wait.h>
int main(int argc, char const *argv[])
{
	int fd_pipe[2];
	int ret =  pipe(fd_pipe);
	if(ret == -1)
		errExit("pipe");

	pid_t fd_fork = fork();

	if(fd_fork < 0)
		errExit("fork");
	else if(fd_fork == 0){
		
		close(fd_pipe[0]);
		char * str = "message from child proess!";
		for(int i=0; i < 6; i ++){
			write(fd_pipe[1], str, strlen(str)+1);
			sleep(1);
		}
		

	}
	else{
		close(fd_pipe[1]);
		char msg[100];

		for (int i = 0; i < 6; ++i)
		{
			memset(msg, '\0', sizeof(msg));
			ssize_t ret_len = read(fd_pipe[0], msg, sizeof(msg));
			if(ret_len > 0){
				msg[ret_len-1] = '\0';
			}
			printf("%s\n", msg);
		}
	}
	
	return 0;
}