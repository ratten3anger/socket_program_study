#include "main.h"

char error_buf[PCAP_ERRORBUF_SIZE] = {0};

void show_usage(char *argv[]){
	printf("[*]Usage:%s -d device\n",argv[0]);
	printf("[*]Help :%s -h\n",argv[0]);
	printf("[*]Example:\n");
	printf("\t%s -d eth0\n",argv[0]);
}

pcap_t * open_device(char *dev){
    //pcap_open_live(const char *device,
    //                     int snaplen,
    //                     int promisc,
    //                     int to_ms,
    //                     char* ebuf)
	pcap_t * handle = pcap_open_live(dev,SNAP_LEN,1,1000,error_buf);
    if(NULL == handle){
        fprintf(stderr,"Couldn't open device %s: %s\n",dev,error_buf);
        return NULL;
    }else{
    	return handle;
    }
}

void got_packet(u_char *args, \
				const struct pcap_pkthdr *header,\
				const u_char *packet){
    

}
