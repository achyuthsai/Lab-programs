#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>


char filename[128];

int main()
{
	int fd, ret;
	char buff[4096];

	sprintf(filename, "/proc/%d/maps", getpid());
	fd = open(filename, O_RDONLY);
	ret = read(fd, buff, sizeof(buff));
	printf("%s\n", buff);
	return 0;
}
