#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<pthread.h>


int sockfd,lisfd,connfd;
struct sockaddr_in servaddr,cliaddr;
int count;

void *thread_func(void *arg)
{
	int cliadr = sizeof(cliaddr);	
		connfd = accept(sockfd,(struct sockaddr *)&cliaddr, &cliadr);
		perror("accept");
		if(connfd < 0) {
			perror("connfd");
			pthread_exit(NULL);
		}
		printf("connection establshed with clint%d\n",count);
		pthread_t tid;
		pthread_create(&tid,NULL,thread_func,NULL);

		char rd_buf[100];
		char wr_buf[100];
		int ret;
		count++;

		printf("send some message to client %d\n", count);
		gets(wr_buf);
		ret = write(connfd,wr_buf,sizeof(wr_buf));
		if(ret<0){
			perror("write");
			exit(1);
		}
		printf("message delivered to client %d\n", count);
		printf("ready to receive data from client %d\n", count);
		ret = read(connfd,rd_buf,sizeof(rd_buf));
		if(ret<0){
			perror("read");
			exit(2);
		}

		printf("message received from cleint %d\n:- %s\n",count,rd_buf);
		pthread_join(tid,NULL);

	return NULL;			
}	
int main()
{


	pthread_t tid;

	sockfd = socket(AF_INET,SOCK_STREAM,0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8001);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	perror("bind");

	lisfd = listen(sockfd,4);
	perror("listen");

	pthread_create(&tid,NULL, thread_func,NULL);

	pthread_join(tid,NULL);
	return 0;
}

	
