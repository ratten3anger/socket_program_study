#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 1234
#define MAXDATASIEZE 128


int main(int argc,char *argv[]){

	int sockfd,num,connectfd;
	char buffer[MAXDATASIEZE] = {0};
	struct hostent *he;
	struct sockaddr_in server;
	int length = 0,index = 0;

	if(argc != 3){
		printf("Ussage:%s <IP address> <message>\n",argv[0]);
		exit(1);
	}

	if((he = gethostbyname(argv[1])) == NULL){
		printf("gethostbyname() error\n");
		exit(1);
	}

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		printf("socket() error\n");
		exit(1);
	}

	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr = *((struct in_addr*)he->h_addr);

	if((connectfd = connect(sockfd,(struct sockaddr *)&server,sizeof(server))) == -1){
		printf("connect() error\n");
		exit(1);
	}

	printf("[*]Send client message:%s\n",argv[2]);
	send(sockfd,argv[2],strlen(argv[2]),0);

	if((num = recv(sockfd,buffer,MAXDATASIEZE,0)) == -1){
		printf("recv() error\n");
		exit(-1);
	}
	//got msg
	buffer[num] = '\0';
	printf("[*]Got server message :%s\n",buffer);
	printf("\n");
	

	close(connectfd);
	close(sockfd);
	return 0;
}