#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>


int main()
{
	char buff[22];
	int fd;

	fd = open("/dev/bufa", O_RDWR); perror("open");

	read(fd, buff, sizeof(buff));perror("read");

	return 0;
}
