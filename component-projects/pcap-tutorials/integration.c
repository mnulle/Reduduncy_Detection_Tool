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

u_int16_t handle_ethernet
        (u_char *args,const struct pcap_pkthdr* pkthdr,const u_char*
        packet);
u_char* handle_IP
        (u_char *args,const struct pcap_pkthdr* pkthdr,const u_char*
        packet);

char * get_ethertype(const u_int16_t ethertype);

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
    u_int16_t type = handle_ethernet(args,pkthdr,packet);
    printf("Packet type: %s\tPacket length: %d\n", get_ethertype(type), pkthdr->len);
    if(type == ETHERTYPE_IP)
    {/* handle IP packet */
        handle_IP(args,pkthdr,packet);
    }else if(type == ETHERTYPE_ARP)
    {/* handle arp packet */
    }
    else if(type == ETHERTYPE_REVARP)
    {/* handle reverse arp packet */
    }
}

u_char* handle_IP
        (u_char *args,const struct pcap_pkthdr* pkthdr,const u_char*
        packet)
{
    const struct my_ip* ip;
    u_int length = pkthdr->len;
    u_int hlen,off,version,p;
    int i;

    int len;

    /* jump pass the ethernet header */
    ip = (struct my_ip*)(packet + sizeof(struct ether_header));
    length -= sizeof(struct ether_header); 

    /* check to see we have a packet of valid length */
    if (length < sizeof(struct my_ip))
    {
        printf("truncated ip %d",length);
        return NULL;
    }

    len     = ntohs(ip->ip_len);
    hlen    = IP_HL(ip); /* header length */
    version = IP_V(ip);/* ip version */
    p       = ip->ip_p;
    /* check version */
    if(version != 4)
    {
      fprintf(stdout,"Unknown version %d\n",version);
      return NULL;
    }

    /* check header length */
    if(hlen < 5 )
    {
        fprintf(stdout,"bad-hlen %d \n",hlen);
    }

    /* see if we have as much packet as we should */
    if(length < len)
        printf("\ntruncated IP - %d bytes missing\n",len - length);

    /* Check to see if we have the first fragment */
    off = ntohs(ip->ip_off);
    if((off & 0x1fff) == 0 )/* aka no 1's in first 13 bits */
    {
        fprintf(stdout,"IP: ");
        fprintf(stdout,"%s ",
                inet_ntoa(ip->ip_src));
        fprintf(stdout,"%s\nHeader length: %d\tIP Packet Length: %d\tOff: %d\n",
                inet_ntoa(ip->ip_dst),
                hlen,len,off);
	switch(p){
	case IPPROTO_TCP:
	    fprintf(stdout,"This is a TCP Packet\n\n");
	    break;
	case IPPROTO_UDP:
	    fprintf(stdout,"This is a UDP Packet\n\n");
	    break;
	default:
	    fprintf(stdout,"This is other packet\n\n");
	    break;
	}
    }

    return NULL;
}

/* handle ethernet packets, much of this code gleaned from
 * print-ether.c from tcpdump source
 */
u_int16_t handle_ethernet
        (u_char *args,const struct pcap_pkthdr* pkthdr,const u_char*
        packet)
{
    u_int caplen = pkthdr->caplen;
    struct ether_header *eptr;  /* net/ethernet.h */
    u_short ether_type;

    if (caplen < ETHER_HDRLEN)
    {
        fprintf(stdout,"Packet length less than ethernet header length\n");
        return -1;
    }

    /* lets start with the ether header... */
    eptr = (struct ether_header *) packet;
    ether_type = ntohs(eptr->ether_type);

    return ether_type;
}

// Returns a string representation of the common Ethertype
char * get_ethertype(const u_int16_t ethertype)
{
    switch(ethertype)
    {
    case ETHERTYPE_IP:
        return "IPv4";
    case ETHERTYPE_IPV6:
        return "IPv6";
    case ETHERTYPE_ARP:
        return "ARP";
    case ETHERTYPE_PUP:
	return "PUP";
    case ETHERTYPE_REVARP:
	return "Reverse ARP";
    case ETHERTYPE_NS:
	return "NS";
    default:
        return "Unknown";
    }
}


//------------------------------------------------------------------- 
int main(int argc, char **argv) 
{ 
  int fnum;
  unsigned int pkt_counter=0;   // packet counter 
  unsigned long byte_counter=0; //total bytes seen in entire trace 
  unsigned long cur_counter=0; //counter for current 1-second interval 
  unsigned long max_volume = 0;  //max value of bytes in one-second interval 
  unsigned long current_ts=0; //current timestamp 
 
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
 
    while (packet = pcap_next(handle,&header)) { 
      // header contains information about the packet (e.g. timestamp) 
      u_char *pkt_ptr = (u_char *)packet; //cast a pointer to the packet data 
      my_callback(NULL, &header, pkt_ptr);
      
      pkt_counter++; //increment number of packets seen 
 
    } //end internal loop for reading packets (all in one file) 
 
    pcap_close(handle);  //close the pcap file 
 
  } //end for loop through each command line argument 
  //---------- Done with Main Packet Processing Loop --------------  
 
  //output some statistics about the whole trace 
  byte_counter /= 1e6;  //convert to MB to make easier to read 
 
  printf("Processed %d packets and %u MBytes, in %d files\n", pkt_counter, byte_counter, argc-1);
  return 0; //done
} //end of main() function

