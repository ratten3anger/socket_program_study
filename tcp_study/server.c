#include <netdb.h>
#include <stdio.h> 
#include <stdlib.h>
#include <strings.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1234
#define BACKLOG 1 
#define MAXLENGTH 128
#define MAXDATASIEZE 128

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


	int socket_fd,connect_fd;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t addrlen;
	char buffer[MAXLENGTH] = {0};
	int num;

	//create tcp socket
	if((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("socket() error");
		exit(1);
	}
	//set socket options
	int opt = SO_REUSEADDR;
	setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(socket_fd,(struct socket *)&server,sizeof(server)) == -1){
		perror("bind() error");
		exit(1);
	}

	if(listen(socket_fd,BACKLOG) == -1){
		perror("listen() error");
		exit(1);
	}

	addrlen = sizeof(client);

	//recv msg and revstr
	while(1){

		if((connect_fd = accept(socket_fd,(struct sockaddr_in*)&client,&addrlen)) == -1){
			perror("accept() error"); 
			continue;
		}else{
			printf("[!]You got a connection: client's ip is:%s,port is %d\n",inet_ntoa(client.sin_addr),htons(client.sin_port));			
		}

		if((num = recv(connect_fd,buffer,MAXDATASIEZE,0)) == -1){
			printf("recv() error\n");
			exit(-1);
		}else{
			buffer[num] = '\0';
			printf("[*]Client msg:%s\n",buffer);
			send(connect_fd,revstr(buffer,num),num,0);
			close(connect_fd);
		}
	}

	close(socket_fd);

	return 0;
}
