#include "udpstudy.h"


int main(int argc,char*argv[]){

	int sockfd,num;
	char buffer[MAXDATASIZE] = {0};
	struct hostent *he;
	struct sockaddr_in server,peer;
	if(argc != 3){
		printf("Usage: %s <IP address> <Message>\n",argv[0]);
		exit(-1);
	}
	if((he = gethostbyname(argv[1])) == NULL){
		perror("gethostbyname error");
		exit(-1);
	}
	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1){
		perror("create socket error");
		exit(-1);
	}
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr = *((struct in_addr*)he->h_addr);
	sendto(sockfd,argv[2],strlen(argv[2]),0,(struct sockaddr*)&server,sizeof(server));
	socklen_t len;
	len	 = sizeof(server);
	while(1){
		if((num = recvfrom(sockfd,buffer,MAXDATASIZE,0,(struct sockaddr *)&peer,&len)) == -1){
			perror("recvfrom error");
			exit(-1);
		}
		//make sure you recevice the right msg;
		if(len != sizeof(server) || memcmp((const void*)&server,(const void *)&peer,len) != 0){
			printf("Recevice msg from other server.\n");
			continue;
		}
		buffer[num] = '\0';
		printf("[*]Server message:%s.\n",buffer);
		break;
	}
	close(sockfd);

	return 0;
}