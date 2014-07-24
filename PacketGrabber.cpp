/***************************************************************
 * File: PacketGrabber.c
 * Author: Matt Nulle
 * This file implements the functions found in PacketGrabber.h
 **************************************************************/

#include "PacketGrabber.h"
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
#include <string.h>

PacketGrabber::PacketGrabber(char *filename) {
	pkt_counter=0;	// packet counter
	payload_counter=0;	// payload counter

	//open the pcap file 
	char errbuf[PCAP_ERRBUF_SIZE]; 			
	handle = pcap_open_offline(filename, errbuf); //call pcap library function 
	
	// Check if it opened properly
	if (handle == NULL) { 
		fprintf(stderr,"Error: PacketGrabber - Couldn't open pcap file %s: %s\n", filename, errbuf); 
	}

	// init packet to null, and allocate memory for payload
	packet = NULL; 
	payload = (char *)malloc((1600)*sizeof(char));
}

PacketGrabber::~PacketGrabber() {
	free(payload); // free payload
	if(handle != NULL) // if file opened, close it
		pcap_close(handle);  //close the pcap file 
}

u_char* PacketGrabber::getNextPacket() {
	packet = pcap_next(handle,&header); // get the next packet, and store it
	pkt_counter++; // increment packet count
	if(packet != NULL) {
		// header contains information about the packet (e.g. timestamp) 
		u_char *pkt_ptr = (u_char *)packet; //cast a pointer to the packet data 
		if (getCurrentPayload()[0] != '\0') payload_counter++; //increment number of payloads seen 
		return pkt_ptr;
	}
	else
		return NULL;
}

void PacketGrabber::resetCounts() {
	// reset counters
	pkt_counter = 0;
	payload_counter = 0;
}

unsigned int PacketGrabber::getPacketCount() {
	return pkt_counter;
}

unsigned int PacketGrabber::getPayloadCount() {
	return payload_counter;
}

char* PacketGrabber::getCurrentPayload() {
	// init payload to \0
	*payload = '\0';
	const struct pcap_pkthdr* pkthdr = &header;

	unsigned char offset = 0;
	
	if(isTCP((char  * )packet)) // only look at TCP packets
	{
		offset = ((((packet)[ETHER_HDRLEN + IPV4_HDRLEN + OFFSET_TCP_DATAOFF]) & 0xf0) / 4);
		int len = pkthdr->len - (offset + ETHER_HDRLEN + IPV4_HDRLEN) + 1;
		if((uint) (offset + ETHER_HDRLEN + IPV4_HDRLEN) != pkthdr->len) { // if it's not all headers
			// These were some debugging prints that are no longer needed (I think)
			// Keeping them here just in case for now
			/*printf("====================DEBUG============================\n");
			//printf("===  Packet:    ");
			//for(uint i = offset + ETHER_HDRLEN + IPV4_HDRLEN; i < pkthdr->len; i++) {
			//	printf("%02X", packet[i]);
			//}
			//printf("  ===\n");	
			printf("===  Pkthdr->len: %d  =====\n", pkthdr->len);
			printf("===  offset:      %d  =====\n", offset);
			printf("===  ETHER_HDRLEN:%d  =====\n", ETHER_HDRLEN);
			printf("===  IPV4_HDRLEN: %d  =====\n", IPV4_HDRLEN);
			printf("===  packetlengh: %d  =====\n", len);*/

			// Copy the payload into the variable for it
			strncpy(payload, (char*) &packet[offset + ETHER_HDRLEN + IPV4_HDRLEN], len);
			// end it with a null, just in case the payload doesn't
			payload[pkthdr->len - (offset + ETHER_HDRLEN + IPV4_HDRLEN)] = '\0';
		}
	}
	else 
		// print no longer necessary, also for debugging, also kept just in case
		;//printf("NOT A TCP\n");
	return payload;
}

char PacketGrabber::isTCP (char * pPacket) {
	// if the bits are set indicating it's TCP, return 1, else 0
	if(pPacket[ETHER_HDRLEN+OFFSET_IPV4_PROTOCOL] == IP_PROTOCOL_TCP) {
		return 1;
	} else {
		return 0;
	}
}

void PacketGrabber::openFile(char* filename) {
	// if there's already one open, close it
	if(handle != NULL)
		pcap_close(handle);  //close the pcap file 
	
	//open the pcap file 
	char errbuf[PCAP_ERRBUF_SIZE]; 
	handle = pcap_open_offline(filename, errbuf);   //call pcap library function 
	 
	if (handle == NULL) { 
		fprintf(stderr,"Error: PacketGrabber - Couldn't open pcap file %s: %s\n", filename, errbuf); 
	} 
}
