#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
 
void handler(int sig){
        printf("recv a sig=%d\n",sig);
                exit(EXIT_SUCCESS);
}
 
 
#define ERR_EXIT(m) \
        do{ \
                perror(m); \
                exit(EXIT_FAILURE);\
        }while(0);
 
int main(int argc,char *argv[]){

		if(argc != 2){
			printf("Usage:%s <Hostname>\n",argv[0]);
			exit(1);
		}
 
        struct hostent *hp;
        if ((hp=gethostbyname(argv[1])) == NULL){
                ERR_EXIT("gethostbyname");
        }
 		//get host & ip
        int i = 0;
        while(hp->h_addr_list[i] != NULL){
                printf("hostname: %s\n",hp->h_name);
                printf("      ip: %s\n",inet_ntoa(*(struct in_addr*)hp->h_addr_list[i]));
                i++;
        }
        //get alias
        char **q;
        for(q = hp->h_aliases; *q !=0 ;q++){
        	printf("Alias : %s\n",*q);
        }
        return 0;
}