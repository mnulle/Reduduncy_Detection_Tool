/***************************************************************
 * File: PacketGrabber.h
 * Author: Matt Nulle
 * This header declares macros and function necessary for
 * getting packets out of a file
 **************************************************************/

#ifndef PACKET_GRABBER
#define PACKET_GRABBER

#include <string>
#include <pcap.h>
#include <stdio.h>
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

// macros for the lengths of sections of different packet types
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

// PacketGrabber class
class PacketGrabber {
public:
	/**
	 * Construct PacketGrabber
	 * args: 
	 *   char *filename: file to pull packets from
	 */
	PacketGrabber(char *filename);

	/**
	 * Releases memory allocated
	 */
	~PacketGrabber();

	/**
	 * Get the next packet out of the file, storing in private variables
	 * return:
	 *   Pointer to the packet. Also stored in the class
	 */
	u_char* getNextPacket();	

	/**
	 * Gets the payload of the current packet
	 * return:
	 *   The payload if the packet is TCP and has one, empty string otherwise. Also stored in the class
	 */
	char* getCurrentPayload();

	/**
	 * Opens a file for reading from the beginning
	 * args: 
	 *   char* filename: name of file to be read from
	 */
	void openFile(char* filename);

	/**
	 * Gets the number of packets read from this file, 
	 * mostly used for verification/debugging.
	 * return:
	 *   Number of packets pulled
	 */
	unsigned int getPacketCount();

	/**
	 * Gets the number of non-empty payloads read from file
	 * return:
	 *   Number of payloads found
	 */
	unsigned int getPayloadCount();

	/**
	 * Reset the values of packetCount and payloadCount
	 */
	void resetCounts();

private:
	/**
	 * Determine if packet is TCP or other format
	 * args: 
	 *   char *pPacket: packet to look at
	 *   int nLength: length of packet
	 * return:
	 *   
	 */
	char isTCP (char * pPacket);

	/* Handle to pcap file opened */
	pcap_t *handle;

	//temporary packet buffers 
	struct pcap_pkthdr header; // The header that pcap gives us 
	const u_char *packet; // The actual packet
	char *payload;

	unsigned int pkt_counter;   // packet counter
	unsigned int payload_counter;   // payload counter
};

#endif
