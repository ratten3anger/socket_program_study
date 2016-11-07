#include "IO_reuse.h"


typedef struct CLIENT{
    int fd;
    char *name;
    struct sockaddr_in addr;
    char *data;
}CLIENT_INFO;

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

void savedata(char *recvbuf,int len,char *data){
	int start = strlen(data);
	for(int i = 0;i < len;i++){
		data[start+i] = recvbuf[i];
	}

}

void process_cli(CLIENT_INFO* client,char* recvbuf,int len){

	recvbuf[len] = '\0';
	if(strlen(client->name) == 0){
		memcpy(client->name,recvbuf,len);
		printf("Client 's name is : %s \n",client->name);
		return;
	}

	printf("Received client(%s) message:%s\n",client->name,recvbuf);
	//save data
	savedata(recvbuf,len,client->data);
	//reverse and send
	send(client->fd,revstr(recvbuf,len),strlen(recvbuf),0);

}


int main(int argc,char *argv[]){

	int i,maxi,maxfd,sockfd;
	int nready;
	ssize_t n;
	fd_set reset,allset;
	int listenfd,connectfd; //socket descriptors
	struct sockaddr_in server;
	//client's info
	CLIENT_INFO client[FD_SETSIZE];
	char recvbuf[MAXDATASIZE] = {0};
	int sin_size;

	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("create socket error");
		exit(-1);
	}
	int opt = SO_REUSEADDR;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind
	if(bind(listenfd,(struct sockaddr *)&server,sizeof(server)) == -1){
		perror("bind error");
		exit(-1);
	}

	//listen
	if(listen(listenfd,BACKLOG) == -1){
		perror("listen error");
		exit(-1);
	}

	sin_size = sizeof(struct sockaddr_in);
	//init for select();
	maxfd = listenfd;
	maxi = -1;
	for(i = 0;i<FD_SETSIZE;i++){
		client[i].fd = -1;
	}
	FD_ZERO(&allset);
	FD_SET(listenfd,&allset);

	while(1){
		struct sockaddr_in addr;
		char *client_name = NULL;
		char *client_data = NULL;
		reset = allset;
		// int select(int maxfdp,fd_set*readset,fd_set*writeset,\
						fd_set* execepset,const struct timeval* timeout);
		nready = select(maxfd+1,&reset,NULL,NULL,NULL);

		if(FD_ISSET(listenfd,&reset)){
			//new client connection
			if((connectfd = accept(listenfd,(struct sockaddr*)&addr,&sin_size)) == -1){
				perror("accpet error");
				continue;
			}

			//put new fd to client
			for(i = 0;i<FD_SETSIZE;i++){
				if(client[i].fd < 0){
					client_name = (char*)malloc(MAXDATASIZE*sizeof(char));
					client_data = (char*)malloc(MAXDATASIZE*sizeof(char));
					client[i].fd = connectfd;
					client[i].name = client_name ;
					client[i].addr = addr;
					client[i].data = client_data;
					client[i].name[0] = '\0';
					client[i].data[0] = '\0';
					printf("[!]You got a connection from :%s,port is %d\n",\
						inet_ntoa(client[i].addr.sin_addr),htons(client[i].addr.sin_port));
				}
			}
			if(i == FD_SETSIZE)
				printf("Too many clients\n");
			FD_SET(connectfd,&allset);
			if(connectfd > maxfd)
				maxfd = connectfd;
			if(i > maxi)
				maxi = i;
			// no more readable desciptors
			if(--nready <= 0)
				continue;
		}
		for(i = 0;i <= maxi ;i++){
			//check all clients for data
			if((sockfd = client[i].fd) < 0)
				continue;
			if(FD_ISSET(sockfd,&reset)){
				if((n = recv(sockfd,recvbuf,MAXDATASIZE,0)) == 0){
					//connection closed by client
					close(sockfd);
					printf("Client(%s) closed connection.User's data:%s \n",\
								client[i].name,client[i].data);
					FD_CLR(sockfd,&allset);
					client[i].fd = -1;
					free(client[i].name);
					free(client[i].data);
				}else{
					process_cli(&client[i],recvbuf,n);
				}
			if(--nready <= 0)
				break;
			}
		}
	}
	close(listenfd);

	return 0;
}