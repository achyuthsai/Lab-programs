#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	int fd, i = 0;
	char buf[100];
	fd = open("/dev/eeprom_device", O_RDWR);
	if(fd < 0) {
		printf("open failed\n");
		exit(0);
	}
	write(fd,"hello",7);
	i = read(fd,buf,7);
	if(i<0)
		perror("read");
	buf[i]='\0';
	printf("application read data %d %s\n",i, buf);
	close(fd);
	return 0;
}
