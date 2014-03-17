#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>


int main()
{
	char buff[100];
	char *buf= "last message";
	int fd,ret;

	fd = open("/dev/bufa", O_RDWR); perror("open");
	lseek(fd,0, SEEK_END);

	ret = write(fd, buf, strlen(buf));perror("write");

	lseek(fd, 0, SEEK_SET);

	read(fd, buff, sizeof(buff));perror("read");

	printf("%s\n", buff);

	return 0;
}
