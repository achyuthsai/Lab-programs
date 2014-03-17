#include<stdio.h>
#include<fcntl.h>


int main()
{

	char buff[17];
	int fd;
	char *buf = "lf";

	fd = open("/dev/myrtc1", O_RDWR);

	write(fd, buf, sizeof(buf));
	
	lseek(fd, 0, SEEK_SET);

	read(fd, buff, sizeof(buff));

	printf("%s\n", buff);

	return 0;
}

