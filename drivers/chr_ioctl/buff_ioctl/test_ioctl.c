#include<stdio.h>
#include<fcntl.h>
#include"chr_ioctl.h"
#include<string.h>



int main()
{

	char buff[100] = "this is the message to driver";
	int fd;
	buffer_t user, user1;

	user1.seek = SEEK_END;
	user1.position = 6;
	int ret;

	strcpy(user.name, buff);
	user.length = strlen(buff);

	printf("before : %s\n", user.name);

	fd = open("/dev/myioctl", O_RDWR);perror("open");

	ret =  ioctl(fd, WRITE_DATA, &user);perror("ioctl");
	ret = ioctl(fd, READ_DATA, &user1); perror("ioctl");

	printf("READ_DATA: %s\n", user1.name);

	return 0;
}

