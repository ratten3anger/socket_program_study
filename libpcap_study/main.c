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
    pcap_t *handle = NULL; 

    if(argc == 1){
    	show_usage(argv);
    	return 1;
    }

    while((opt = getopt(argc,argv,"hd:")) != -1){
    	switch(opt){
    		case 'h':
    			show_usage(argv);
    			return 1;
    		case 'd':
    			strncpy(dev,optarg,DEVICENAME);
    			break;
    	}
    }

    if((handle = open_device(dev)) != NULL){
    	printf("ok\n");
    }else{
    	printf("error\n");
    	return -1;
    }
    //set callback function
    //int pcap_loop(pcap_t *p,
	//				int cnt,
	//				pcap_handler callback,
	//				u_char *user );
    pcap_loop(handle,-1,got_packet,NULL);
    
    pcap_close(handle);
    return 0;
}