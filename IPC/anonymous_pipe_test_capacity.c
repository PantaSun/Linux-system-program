/**
 * 测试匿名管道容量
 * PantaSun
 * 2019-09-10
 */


#include <unistd.h>
#include <stdio.h>
#include "tlpi_hdr.h"
#include <string.h>
#include <fcntl.h>


int main(int argc, char const *argv[])
{
	int fd_pipe[2];
	int ret =  pipe(fd_pipe);
	if(ret == -1)
		errExit("pipe");

	int ret_len;
	int capacity = 0;
	// 获取文件描述符当前属性
	int flags = fcntl(fd_pipe[1], F_GETFL);
	// 设为非阻塞
	fcntl(fd_pipe[1], F_SETFL, flags|O_NONBLOCK);

    // 不断的向写端写入数据（写端的编号为1）
    // 直到无法写入为止（write在非阻塞情况下，直接返回）
	while(1){
		ret_len = write(fd_pipe[1], "t", 1);
		if(ret_len < 0){
			perror("write");
			break;
		}
		capacity += ret_len;
	}

	printf("pipe capacity: %d\n", capacity);

	return 0;
}