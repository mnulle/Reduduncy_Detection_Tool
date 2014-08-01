//  pcap_throughput
//
//   reads in a pcap file and outputs basic throughput statistics 

#include <stdio.h>
#include <pcap.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/if_ether.h> 
#include <net/ethernet.h>
#include <netinet/ether.h> 
#include <netinet/ip.h> 
#include <unistd.h>

#define OFFSET_IPV4_PROTOCOL	9
#define IP_PROTOCOL_TCP		6
#define IPV4_HDRLEN 		20
#define OFFSET_TCP_DATAOFF	12

//defines for the packet type code in an ETHERNET header
#define ETHER_TYPE_IP (0x0800)
#define ETHER_TYPE_8021Q (0x8100)

/* tcpdump header (ether.h) defines ETHER_HDRLEN) */
#ifndef ETHER_HDRLEN 
#define ETHER_HDRLEN 14
#endif

#ifndef ETHERTYPE_NS
#define ETHERTYPE_NS 0x0600
#endif


char * get_ethertype(const u_int16_t ethertype);
char		isTCP (char * pPacket, int nLength);

/*
 * Structure of an internet header, naked of options.
 *
 * Stolen from tcpdump source (thanks tcpdump people)
 *
 * We declare ip_len and ip_off to be short, rather than u_short
 * pragmatically since otherwise unsigned comparisons can result
 * against negative integers quite easily, and fail in subtle ways.
 */
struct my_ip {
	u_int8_t	ip_vhl;		/* header length, version */
#define IP_V(ip)	(((ip)->ip_vhl & 0xf0) >> 4)
#define IP_HL(ip)	((ip)->ip_vhl & 0x0f)
	u_int8_t	ip_tos;		/* type of service */
	u_int16_t	ip_len;		/* total length */
	u_int16_t	ip_id;		/* identification */
	u_int16_t	ip_off;		/* fragment offset field */
#define	IP_DF 0x4000			/* dont fragment flag */
#define	IP_MF 0x2000			/* more fragments flag */
#define	IP_OFFMASK 0x1fff		/* mask for fragmenting bits */
	u_int8_t	ip_ttl;		/* time to live */
	u_int8_t	ip_p;		/* protocol */
	u_int16_t	ip_sum;		/* checksum */
	struct	in_addr ip_src,ip_dst;	/* source and dest address */
};


/* looking at ethernet headers */
void my_callback(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char*
        packet)
{
    unsigned char offset;
    int i, formatcount = 0;

    if(isTCP((char  * )packet, pkthdr->len))
    {
	offset = ((((packet)[ETHER_HDRLEN + IPV4_HDRLEN + OFFSET_TCP_DATAOFF]) & 0xf0) / 4);
	if(offset + ETHER_HDRLEN + IPV4_HDRLEN != pkthdr->len) {
		printf("TCP of length: %d\n", pkthdr->len);	
		printf("Total HDR LEN: %d\n\n", offset + ETHER_HDRLEN + IPV4_HDRLEN);
		printf("Payload:\n");
		for(i = offset + ETHER_HDRLEN + IPV4_HDRLEN; i < pkthdr->len; i++) {
			if(packet[i] == '\n' || packet[i] == '\r')
				printf(" ");
			else 
				printf("%c", packet[i]);
			formatcount++;
			if(formatcount == 8)
				printf(" ");
			else if(formatcount == 16) {
				formatcount = 0;
				printf("\n");
			}
		}
		printf("\n");
	}
    }

}

char isTCP (char * pPacket, int nLength) {
	if(pPacket[ETHER_HDRLEN+OFFSET_IPV4_PROTOCOL] == IP_PROTOCOL_TCP) {
		return 1;
	} else {
		return 0;
	}
}


//------------------------------------------------------------------- 
int main(int argc, char **argv) 
{ 
  int fnum;
  unsigned int pkt_counter=0;   // packet counter
 
  //temporary packet buffers 
  struct pcap_pkthdr header; // The header that pcap gives us 
  const u_char *packet; // The actual packet 
  
  //check command line arguments 
  if (argc < 2) { 
    fprintf(stderr, "Usage: %s [input pcaps]\n", argv[0]); 
    exit(1); 
  } 
  
  //-------- Begin Main Packet Processing Loop ------------------- 
  //loop through each pcap file in command line args 
  for (fnum=1; fnum < argc; fnum++) {  
 
    //----------------- 
    //open the pcap file 
    pcap_t *handle; 
    char errbuf[PCAP_ERRBUF_SIZE]; //not sure what to do with this, oh well 
    handle = pcap_open_offline(argv[fnum], errbuf);   //call pcap library function 
 
    if (handle == NULL) { 
      fprintf(stderr,"Couldn't open pcap file %s: %s\n", argv[fnum], errbuf); 
      return(2); 
    } 
 
    //----------------- 
    //begin processing the packets in this particular file, one at a time 
 
    while ((packet = pcap_next(handle,&header))) { 
      // header contains information about the packet (e.g. timestamp) 
      u_char *pkt_ptr = (u_char *)packet; //cast a pointer to the packet data 
      my_callback(NULL, &header, pkt_ptr);
      
      pkt_counter++; //increment number of packets seen 
 
    } //end internal loop for reading packets (all in one file) 
 
    pcap_close(handle);  //close the pcap file 
 
  } //end for loop through each command line argument 
  //---------- Done with Main Packet Processing Loop --------------  
 
  //output some statistics about the whole trace 
 
  printf("Processed %d packets in %d files\n", pkt_counter, argc-1);
  return 0; //done
} //end of main() function

