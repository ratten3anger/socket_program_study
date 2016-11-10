#include "multithreadstudy.h"

struct ARG{
	int connfd;
	struct sockaddr_in client;
};

char *revstr(char *str, size_t len){

    char    *start = str;
    char    *end = str + len - 1;
    char    ch;

    if (str != NULL){
        while (start < end){
            ch = *start;
            *start++ = *end;
            *end-- = ch;
        }
    }
    return str;
}

void process_cli(int connfd,struct sockaddr_in client){
	int num;
	char recvbuf[MAXDATASIZE] = {0};
	char sendbuf[MAXDATASIZE] = {0};
	char cli_name[MAXDATASIZE] = {0};
	printf("[!]You got a connection from :%s,port is %d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));

	num = recv(connfd,cli_name,MAXDATASIZE,0);
	if(num == 0){
		close(connfd);
		printf("Client disconnected!\n");
		return;
	}
	cli_name[num] = '\0';
	printf("[*]Client's name is : %s\n",cli_name);
	while(num = recv(connfd,recvbuf,MAXDATASIZE,0)){
		recvbuf[num] = '\0';
		if(!strncmp(recvbuf,"quit",4)){
			printf("[!]Client exit.\n");
			send(connfd,"Connection closed",strlen("Connection closed"),0);
			break;
		}else{
			printf("[*]Received client(%s) message:%s\n", cli_name,recvbuf);
			send(connfd,revstr(recvbuf,num),num,0);
		}
	}
	close(connfd);
}

void *function(void *arg){
	struct ARG *info;
	info = (struct ARG*)arg;
	process_cli(info->connfd,info->client);
	free(arg);
	pthread_exit(NULL);
}

int main(int argc,char *argv[]){

	int sockfd,connfd;
	pthread_t ptid;
	struct ARG *arg;
	struct sockaddr_in server,client;
	socklen_t len;

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("Create socket error");
		exit(-1);
	}
	//addr resue settings
	int opt = SO_REUSEADDR;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind
	if(bind(sockfd,(struct sockaddr *)&server,sizeof(server)) == -1){
		perror("bind error");
		exit(-1);
	}

	//listen
	if(listen(sockfd,BACKLOG) == -1){
		perror("listen error");
		exit(-1);
	}

	len = sizeof(client);
	//handle connections
	while(1){
		if((connfd = accept(sockfd,(struct sockaddr *)&client,&len)) == -1){
			perror("accept	error");
			exit(-1);
		}

		arg = (struct ARG*)malloc(sizeof(struct ARG));
		arg->connfd = connfd;
		//I feel uncomfortable when I use this func...
		memcpy((void*)&arg->client,&client,sizeof(client));
		if(pthread_create(&ptid,NULL,function,(void*)arg)){
			perror("pthread_create error");
			exit(-1);
		}

	}
	close(sockfd);

	return 0;
}
