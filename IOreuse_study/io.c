#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAXDATALEN 255

int main(int argc, char *argv[])
{
	int maxfd, sockfd;
	int n, socklen;
	char msg[MAXDATALEN+1];

	struct sockaddr_in peeraddr, localaddr;
	fd_set infds;

	if(argc != 5){
		printf(" %s <dest IP address> <dset port> <source IP address> <source port>\n",argv[0]);
		exit(-1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0){
		perror("socket() error.\n");
		exit(1);
	}

	int opt = SO_REUSEADDR;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	socklen = sizeof(struct sockaddr_in);
	memset(&peeraddr, 0, socklen);
	peeraddr.sin_family = AF_INET;
	peeraddr.sin_port = htons(atoi(argv[2]));

	if(inet_pton(AF_INET, argv[1], &peeraddr.sin_addr) <= 0){
		perror("dest IP wrong.\n");
		exit(1);
	}

	memset(&localaddr, 0, socklen);
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(atoi(argv[4]));

	if(inet_pton(AF_INET, argv[3], &peeraddr.sin_addr) <= 0){
		perror("source IP wrong.\n");
		exit(1);
	}
	if(bind(sockfd, (struct sockaddr *)&localaddr, socklen) < 0){
		perror("bind() error.\n");
		exit(1);
	}

	connect(sockfd, (struct sockaddr*)&peeraddr, socklen);
	
	while(1){
		FD_ZERO(&infds);
		FD_SET(fileno(stdin), &infds);
		FD_SET(sockfd, &infds);
		maxfd = (fileno(stdin) > sockfd ? fileno(stdin):sockfd) +1;
		if(select(maxfd, &infds, NULL, NULL, NULL) == -1){
			fprintf(stderr, "select error\n");
			exit(-1);
		}

		if(FD_ISSET(sockfd, &infds)){
			n = read(sockfd, msg, MAXDATALEN);
			if((n == -1) || (n == 0)){
				printf("peer closed\n");
				exit(-1);
			}
			else{
				msg[n] = 0;
				printf("Msg from dest: %s\n", msg);
			}
		}

		if(FD_ISSET(fileno(stdin), &infds)){
			fgets(msg, MAXDATALEN, stdin);
			msg[strlen(msg)-1] = '\0';
			if(strlen(msg) == 0){
				printf("talk closed..\n");
				exit(0);
			}
			write(sockfd, msg, strlen(msg));
		}
	}
	return 0;
}
