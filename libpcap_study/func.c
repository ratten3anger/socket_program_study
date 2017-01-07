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

//reference : http://www.tcpdump.org/sniffex.c
void got_packet(u_char *args, \
				const struct pcap_pkthdr *header,\
				const u_char *packet){
	
	int size_ip;
    static int packet_count = 1;
    const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
	const struct sniff_ip *ip;              /* The IP header */

	printf("\nPacket number %d:\n", packet_count);
	packet_count++;	
	//got ethernet header && ip header
	ethernet = (struct sniff_ethernet*)(packet);
	//printf("%04x",ntohs(ethernet->ether_type));
	switch(ntohs(ethernet->ether_type)){
		case ETHER_IP_PROTO:
			printf("\t----IP----\n");
			break;
		case ETHER_ARP_PROTO:
			printf("\t----ARP----\n");
			return;
		default:
			printf("\t----unknown----\n");
	}
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	size_ip = IP_HL(ip)*4;
	if (size_ip < 20) {
		printf("   * Invalid IP header length: %u bytes\n", size_ip);
		return;
	}

	/* print source and destination IP addresses */
	printf("       From: %s\n", inet_ntoa(ip->ip_src));
	printf("         To: %s\n", inet_ntoa(ip->ip_dst));

	//protocol
	switch(ip->ip_p) {
		case IPPROTO_TCP:
			printf("   Protocol: TCP\n");
			break;
		case IPPROTO_UDP:
			printf("   Protocol: UDP\n");
			return;
		case IPPROTO_ICMP:
			printf("   Protocol: ICMP\n");
			return;
		case IPPROTO_IP:
			printf("   Protocol: IP\n");
			return;
		default:
			printf("   Protocol: unknown\n");
			return;
	}
}
int send_menu(){
	char opt[5];
	printf("[*]Plz choice packet you want to send:\n");
	printf("1. ARP\n");
	printf(">");
	read(0,opt,5);
	return atoi(opt);
}

//ICMP packet,just for test
u_char data[142] = {0x00,0xd0,0xd0,0xa1,0x1f,0x40,0x00,0x10,0xdb,0x3f,0xa1,0x35,0x08,0x00,\
            0x45,0x00,0x00,0x80,0x95,0x11,0x00,0x00,0x3f,0x01,0x8b,0x5b,0xc2,0x88,0x51,0xde,\
            0x46,0x00,0x00,0xaa,0x00,0x00,0x32,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,\
        11,12,13,14,15,16,17,18,19,20,\
        21,22,23,24,25,26,27,28,29,30,\
        31,32,33,34,35,36,37,38,39,40,\
        41,42,43,44,45,46,47,48,49,50,\
        51,52,53,54,55,56,57,58,59,60,\
        61,62,63,64,65,66,67,68,69,70,\
        71,72,73,74,75,76,77,78,79,80,\
        81,82,83,84,85,86,87,88,89,90,\
        91,92,93,94,95,96,97,98,99,100};

void send_arp_packet(pcap_t * handle){
	//printf("I will send ARP PACKET!\n");
	for(int i = 0;i<1000;i++){
		 pcap_sendpacket(handle,data,142);
	}
}