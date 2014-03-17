#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<strings.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int count;

int main()
{

	int sockfd,lisfd,clifd,connfd;
	int pid;

	struct sockaddr_in servaddr,cliaddr;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
		if(sockfd<0){
			perror("socket");	
			exit(1);
		}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8000);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(sockfd,(struct sockaddr *)&servaddr, sizeof(servaddr));

	lisfd = listen(sockfd,4);

	here:
	pid = fork();

	if(pid!=0) {

		clifd = sizeof(cliaddr);
		connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clifd);
		if(connfd < 0){
			perror("accept");
			exit(2);
		}
		count++;
		printf("connection established with client %d\n", count);
		getchar();
	}				

	if(pid==0) {
		clifd = sizeof(cliaddr);
		connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clifd);	
		if(connfd < 0){
			perror("accept");
			exit(3);
		}
		count++;
		printf("connection established with client %d\n",count);
		getchar();
		goto here;
	}

	return 0;

}

