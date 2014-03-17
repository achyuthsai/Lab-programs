#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include<strings.h>
#include<string.h>


int main()
{

	int sockfd, ret;

	sockfd = socket(AF_INET,SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	char buff[20];
	char buf[20];

	char *serv_ip = "127.0.0.1";
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8000);

	inet_pton(AF_INET,serv_ip,&servaddr.sin_addr.s_addr);


	ret = connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	if(ret!=0)
		{perror("connect");
		exit(1);
	}
	printf("connection established from server\n");
	printf("ready to read data ...\n");

	ret = read(sockfd,buf,sizeof(buf));
	if(ret<0){	
		perror("read");	
		exit(2);
	}

	printf("got message: %s\n", buf);
	printf("give message to server..\n");
	gets(buff);
	ret = write(sockfd,buff,sizeof(buff));
	if(ret<0){	
		perror("write");	
		exit(3);
	}
	
	printf("message delivered\n");

	return 0;
}


