#include "IO_reuse.h"


char *getMessage(char *sendline,int len,FILE *fp){
	printf("[-]Input string to server :");
	return(fgets(sendline,MAXDATASIZE,fp));
}


void process(FILE *fp,int sockfd){
	char sendline[MAXDATASIZE] = {0};
	char recvline[MAXDATASIZE] = {0};
	int num;
	printf("[!]Connect to server\n");
	printf("[-]Input the client's name:");
	//read info and send
	read(0,sendline,MAXDATASIZE-1);
	send(sockfd,sendline,strlen(sendline),0);

	while(getMessage(sendline,MAXDATASIZE,fp) != NULL){
		send(sockfd,sendline,strlen(sendline),0);
		if((num = recv(sockfd,recvline,MAXDATASIZE,0)) == 0){
			printf("[!]Server terminated\n");
			return;
		}
		recvline[num] = '\0';
		printf("[*]Server message:%s\n",recvline);
	}
	printf("\nExit\n");
}


int main(int argc,char *argv[]){

	int sockfd;
	struct hostent *he;
	struct sockaddr_in server;
	setbuf(stdin,NULL);
	setbuf(stdout,NULL);

	if(argc != 2){
		printf("Usage:%s <IP address>\n",argv[0] );
		exit(1);
	}

	if((he = gethostbyname(argv[1])) == NULL){
		perror("gethostbyname error");
		exit(-1);
	}

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("create socket error");
		exit(-1);
	}

	//fill 
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr = *((struct in_addr*)he->h_addr);

	//connect
	if(connect(sockfd,(struct sockaddr *)&server,sizeof(server)) == -1){
		perror("connect error");
		exit(-1);
	}
	process(stdin,sockfd);
	close(sockfd);

	return 0;
}