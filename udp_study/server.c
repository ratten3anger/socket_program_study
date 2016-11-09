#include "udpstudy.h"

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

int main(int argc,char *argv[]){

	int sockfd;
	struct sockaddr_in server,client;
	socklen_t len;
	int num;
	char buffer[MAXDATASIZE];
	
	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1){
		perror("Create socket error");
		exit(-1);
	}
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

	len = sizeof(client);
	//recv
	while((num = recvfrom(sockfd,buffer,MAXDATASIZE,0,(struct sockaddr *)&client,&len)) != -1){
		printf("[!]You got connetion from client %s , port is %d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
		buffer[num] = '\0';
		printf("[*]Got client message:%s\n",buffer);
		if(!strncmp(buffer,"quit",4)){
			printf("[!]Client exit\n");
			sendto(sockfd,"exit",4,0,(struct sockaddr *)&client,len);
			break;
		}
		sendto(sockfd,revstr(buffer,num),num,0,(struct sockaddr *)&client,len);
	}
	close(sockfd);


    return 0;
}
