/*************************************************************************
	> File Name: main.c
	> Author: muhe
	> Mail: o0xmuhe@gmail.com
	> Created Time: Sat 31 Dec 2016 05:15:39 PM CST
 ************************************************************************/
#include "main.h"

int main(int argc,char *argv[]){

    char *dev = (char *)malloc(DEVICENAME * sizeof(char));
    int opt = 0;
	int send_enable = 0;
    pcap_t *handle = NULL; 

	setbuf(stdin,NULL);
	setbuf(stdout,NULL);

    if(argc == 1){
    	show_usage(argv);
    	return 1;
    }

    while((opt = getopt(argc,argv,"hd:s")) != -1){
    	switch(opt){
    		case 'h':
    			show_usage(argv);
    			return 1;
			case 's':
				send_enable = 1;
				break;
    		case 'd':
    			strncpy(dev,optarg,DEVICENAME);
    			break;
    	}
    }

    if((handle = open_device(dev)) != NULL){
    	printf("[*]ok...\n");
    	printf("[*]Device : %s\n",argv[2]);
    }else{
    	printf("[!]error\n");
    	return -1;
    }
    //set callback function
    //int pcap_loop(pcap_t *p,
	//				int cnt,
	//				pcap_handler callback,
	//				u_char *user );
	if(!send_enable){
		printf("[*]Begin to capture packet...\n");
    	pcap_loop(handle,-1,got_packet,NULL);
	}else{
		while(1){
			switch(send_menu()){
				case 1:
					send_arp_packet(handle);
					break;
				default:
					printf("Other type not suporrt now!\n");
					break;
			}
		}
	}
    pcap_close(handle);
    return 0;
}