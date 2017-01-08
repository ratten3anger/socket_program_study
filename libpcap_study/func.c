#include "main.h"

char error_buf[PCAP_ERRORBUF_SIZE] = {0};

void show_usage(char *argv[]){
	printf("[*]Usage:%s -d device [-s]\n",argv[0]);
	printf("[*]Help :%s -h\n",argv[0]);
	printf("\tsniffer will send packet if -s flag is setted\n");
	printf("[*]Example:\n");
	printf("\t%s -d eth0\n",argv[0]);
	printf("\t%s -d eth0 -s\n",argv[0]);
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
    const struct sniff_ethernet *ethernet = NULL;  /* The ethernet header [1] */
	const struct sniff_ip *ip = NULL;              /* The IP header */
	struct arphdr *arpheader = NULL;

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
			arpheader = (struct arphdr *)(packet + SIZE_ETHERNET);
			printf("	Hardware type: %s\n", (ntohs(arpheader->htype) == 1) ? "Ethernet" : "Unknown");
			printf("	Protocol type: %s\n", (ntohs(arpheader->ptype) == 0x0800) ? "IPv4" : "Unknown");
			printf("	Operation: %s\n", (ntohs(arpheader->oper) == ARP_REQUEST)? "ARP Request" : "ARP Reply");
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

//ARP packet for test
u_char data[42] = { 0xff,0xff,0xff,0xff,0xff,0xff,0x30,0x10,0xb3,0x22,0x6c,0x84,0x08,0x06,0x00,0x01,\
					0x08,0x00,0x06,0x04,0x00,0x01,0x30,0x10,0xb3,0x22,0x6c,0x84,0xc0,0xa8,0x01,0x78,\
					0xff,0xff,0xff,0xff,0xff,0xff,0xc0,0xa8,0x00,0x72};

void send_arp_packet(pcap_t * handle){
	printf("I will send ARP PACKET!\n");

	for(int i = 0;i<100;i++){
		 pcap_sendpacket(handle,data,42);
	}
}
