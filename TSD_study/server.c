#include "TSD.h"


pthread_key_t key;
pthread_once_t once = PTHREAD_ONCE_INIT;


struct ARG{
	int connfd;
	struct sockaddr_in client;
};

struct ST_DATA{
	int index;

};

//destructor
static void destructor(void *ptr){
	free(ptr);
}

static void create_once(void){
	pthread_key_create(&key,destructor);
}

void savedata_r(char *recvbuf,int len,char *cli_data){
	struct ST_DATA *data;
	int i = 0;
	//make sure every thread just invoke init() once
	pthread_once(&once,create_once);
	//get binding vaule
	if((data = (struct ST_DATA*)pthread_getspecific(key)) == NULL){
		data = (struct ST_DATA*)malloc(sizeof(struct ST_DATA));
		pthread_setspecific(key,data);
		data->index = 0;
	}
	for(;i<len-1;i++){
		cli_data[data->index++] = recvbuf[i];
	}
	cli_data[data->index] = '\0';
}

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
	char cli_data[MAXDATASIZE] = {0};

	printf("[!]You got a connection from :%s,port is %d\n",inet_ntoa(client.sin_addr),htons(client.sin_port));

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
		printf("[*]Received client(%s) message:%s\n", cli_name,recvbuf);
		//TSD,save data for every thread
		savedata_r(recvbuf,num,cli_data);
		send(connfd,revstr(recvbuf,num),num,0);
	}
	close(connfd);
	printf("Client(%s) closed connection.\nUser data:%s\n",cli_name,cli_data);
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