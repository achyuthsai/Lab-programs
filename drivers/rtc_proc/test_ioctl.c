#include<stdio.h>
#include<unistd.h>
#include"header.h"
#include<stdlib.h>
#include<fcntl.h>

int main()
{

	int ret,fd;
	int arg;
	char *buff = "7/11/13";
	int date = 0x05;

	fd = open("/proc/rtc_proc/rtc_date", O_RDWR);
	if(fd<0)
		perror("open");
	
	ret = write(fd,buff, strlen(buff));perror("write");	
	

	return 0;
}

