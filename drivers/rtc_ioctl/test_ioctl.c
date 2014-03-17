#include<stdio.h>
#include<unistd.h>
#include"header.h"
#include<stdlib.h>
#include<fcntl.h>

int main()
{

	int ret,fd;
	int arg;
	int date = 0x05;

	fd = open("/dev/rtc_ioctl", O_RDWR);
	if(fd<0)
		perror("open");
	
	ret = ioctl(fd, GET_DATE, &arg);
	printf("before changed, date: %x\n", (unsigned)arg);
	
	ret = ioctl(fd, SET_DATE, &date);
	printf("%d\n",ret);
	
	ret = ioctl(fd, GET_DATE, &arg);
	printf("after changed, date: %x\n", (unsigned)arg);

	return 0;
}

