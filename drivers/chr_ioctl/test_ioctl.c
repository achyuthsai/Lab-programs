#include<stdio.h>
#include<fcntl.h>
#include"chr_ioctl.h"



int main()
{

	char buff[100];
	int fd;
	int size;

	fd = open("/dev/myioctl", O_RDWR);

	ioctl(fd, FILL_ZERO);

	read(fd, buff, sizeof(buff));
	
	printf("%s\n", buff);

	ioctl(fd, FILL_CHAR, 65);
	read(fd, buff, sizeof(buff));
	printf("%s\n", buff);

	ioctl(fd, SET_SIZE, 3012);
	ioctl(fd, GET_SIZE, &size);

	printf("%d\n", size);
	return 0;
}

