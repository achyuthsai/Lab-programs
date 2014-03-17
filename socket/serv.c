#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include<strings.h>
#include<string.h>
#include<stdlib.h>

int main()
{

	int sockfd,lisfd,connfd;
	struct sockaddr_in servaddr,cliaddr;
	char buff[20];
	char buf[20];
	int n;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		perror("socket");
		exit(1);
	}

	bzero((struct sockaddr*)&servaddr,sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8000);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockfd, (struct sockaddr*)&servaddr,sizeof(servaddr));

	lisfd = listen(sockfd, 4);

	while(1) {

		int clilen = sizeof(cliaddr);

		connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
		if(connfd<0){
			perror("accept");
			exit(1);
		}
		printf("client connected\n");
		printf("send some message to client\n");
		gets(buff);
		n = write(connfd,buff,sizeof(buff));
		if(n<0){
			perror("write");
			exit(2);
		}
		printf("message delivered to client\n");
		n = read(connfd,buf,sizeof(buf));
		if(n<0){
			perror("write");
			exit(3);
		}
		printf("received message from client:%s\n",buf);
			

	}

	return 0;
}


	


	
