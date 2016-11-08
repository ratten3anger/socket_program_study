#include "udpstudy.h"


int main(int argc,char*argv[]){

	int sockfd,num;
	char buffer[MAXDATASIZE] = {0};
	char sendbuffer[MAXDATASIZE] = {0};
	struct hostent *he;
	struct sockaddr_in server,peer;
	setbuf(stdin,0);
	setbuf(stdout,0);
	if(argc != 2){
		printf("Usage: %s <IP address>",argv[0]);
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
	//recv && send
	while(1){
		printf("[-]Send data to server:");
		read(0,sendbuffer,MAXDATASIZE-1);
		sendto(sockfd,sendbuffer,strlen(sendbuffer),0,(struct sockaddr*)&server,sizeof(server));
		socklen_t len;
		len	 = sizeof(server);
		if((num = recvfrom(sockfd,buffer,MAXDATASIZE,0,(struct sockaddr *)&peer,&len)) == -1){
			perror("recvfrom error");
			break;
		}
		//make sure you recevice the right msg;
		if(len != sizeof(server) || memcmp((const void*)&server,(const void *)&peer,len) != 0){
			printf("Recevice msg from other server.\n");
			continue;
		}
		buffer[num] = '\0';
		if(!strncmp(buffer,"exit",4)){
			close(sockfd);
			exit(0);
		}
		printf("[*]Server message:%s.\n",buffer);
	}
	close(sockfd);

	return 0;
}