#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>


int main()
{
	int ret, fd;
	loff_t ofset;
	char buff[200];
	char sbuff[100];


	memset(sbuff, 'A', sizeof(sbuff) -1);
//	sbuff[99] = '\0';

	fd = open("/dev/vbuff", O_RDWR); perror("open");
	
	ret = write(fd, sbuff, sizeof(sbuff));perror("write");
	printf("written %d bytes\n", ret);

	memset(sbuff, 'B', sizeof(sbuff) -1);
	ret = write(fd, sbuff, sizeof(sbuff));perror("write");
	printf("written %d bytes\n", ret);
/*
	memset(sbuff, 'C', sizeof(sbuff) -1);
	ret = write(fd, sbuff, sizeof(sbuff));perror("write");
	printf("written %d bytes\n", ret);
*/
	ofset = lseek(fd, 0, SEEK_SET);

	ret = read(fd, buff, sizeof(buff));
	printf("data: %s\n", buff);
	printf("offset: %lu\n", ofset);	


	return 0;
}

