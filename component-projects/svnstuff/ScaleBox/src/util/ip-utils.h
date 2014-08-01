/* ip-utils.h
 **********************************************************
 * This code is part of the Scalability in a Box (ScaleBox)
 * system developed at the University of Notre Dame. 
 * 
 * For additional information, please visit the NetScale
 * laboratory webpage at:
 *    http://gipse.cse.nd.edu/NetScale
 *
 * PI:       Dr. Aaron Striegel    striegel@nd.edu
 * 
 * Students: Dave Salyers (lead)   Yingxin Jiang
 *           Xiaolong Li           Jeff Smith
 * 
 **********************************************************
 * $Revision: 1.3 $  $Date: 2008/04/09 02:53:17 $
 **********************************************************
 */

#ifndef IPUTILS_H_
#define IPUTILS_H_

#include <string>
using namespace std;

#include "../core/StdHeaders.h"

#include "net-const.h"

#define ETHERNET_LENGTH		14
#define L2_OFFSET			14

#define OFFSET_ETH_DSTMAC		0
#define OFFSET_ETH_SRCMAC		6
#define OFFSET_ETH_TYPELEN		12

#define ETH_TYPELEN_IP_B1		(char) 0x08
#define ETH_TYPELEN_IP_B2		(char) 0x00

#define ETH_TYPELEN_ARP_B1		(char) 0x08
#define ETH_TYPELEN_ARP_B2		(char) 0x06

#define DEFAULT_MAC_LEN 		6
#define DEFAULT_IP_LEN			4


#define IP_PROTOCOL_ICMP	1
#define IP_PROTOCOL_IGMP	2
#define IP_PROTOCOL_TCP		6
#define IP_PROTOCOL_UDP		17
#define IP_PROTOCOL_IPSEC_ESP	50
#define IP_PROTOCOL_IPSEC_AH	51

#define OFFSET_IPV4_VERLEN		0
#define OFFSET_IPV4_TOS			1
#define OFFSET_IPV4_LENGTH		2
#define OFFSET_IPV4_ID				4
#define OFFSET_IPV4_FLAGS			6
#define OFFSET_IPV4_FRAG			6
#define OFFSET_IPV4_TTL			8
#define OFFSET_IPV4_PROTOCOL		9
#define OFFSET_IPV4_CHECKSUM		10
#define OFFSET_IPV4_PROTOCOL_SRCIP		12
#define OFFSET_IPV4_PROTOCOL_DSTIP		16

#define L3_IPV4_OFFSET			20

#define OFFSET_TCP_SRCPORT		0
#define OFFSET_TCP_DSTPORT		2
#define OFFSET_TCP_SEQNUM		4
#define OFFSET_TCP_ACKNUM		8
#define OFFSET_TCP_DATAOFF	12
#define OFFSET_TCP_CONTROL	13
#define OFFSET_TCP_WINDOW		14
#define OFFSET_TCP_CKSUM		16


#define BITMASK_TCP_FLAG_URG		0x20
#define BITMASK_TCP_FLAG_ACK		0x10
#define BITMASK_TCP_FLAG_PSH		0x08
#define BITMASK_TCP_FLAG_RST		0x04
#define BITMASK_TCP_FLAG_SYN		0x02
#define BITMASK_TCP_FLAG_FIN		0x01

#define BITMASK_TCP_FLAG_INV_ACK	0xEF
#define BITMASK_TCP_FLAG_INV_PSH	0xF7

#define OFFSET_UDP_DATA				8
#define OFFSET_IPV4_DATA				14+20+20



/// @brief Determine if the packet is IP or a different type
/// This function assumes pPacket is of sufficient length (>= 14 bytes)
/// and is an Ethernet packet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns 1 if the packet is IP (true), 0 (false) otherwise
char		isPacketIP (char * pPacket, int nLength);

/// @brief Check an IPv4 packet to see if it is carrying IPsec packets
/// Determine if a packet is IPsec within an IPv4 packet.  It is assumed
/// that the packet is already verified to have been IPv4 before this
/// function is called and this function will only be checking the
/// IP protocol field for the presence of an AH or ESP style header
/// @returns 1 if the packet is indeed IPsec, 0 otherwise
char		isPacketIPv4IPsec (char * pPacket, int nLength);


/// @brief Determine if the packet is ARP or a different type
/// This function assumes pPacket is of sufficient length (>= 14 bytes)
/// and is an Ethernet packet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns 1 if the packet is ARP (true), 0 (false) otherwise
char		isPacketARP (char * pPacket, int nLength);

/// @brief Retrieve the IP version from the packet
/// This function assumes that sufficient length exists in the packet
/// (>= 34 bytes, MAC+IP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns The IP version in the IP header 
char		getIPVersion (char * pPacket, int nLength);


/// @brief Determine if the packet is TCP
/// This function assumes that sufficient length exists in the packet
/// (>= 34 bytes, MAC+IP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns 1 (true) if the protocol is TCP, other 0 (false) 
char		isTCP (char * pPacket, int nLength);

/// @brief Determine if the packet is UDP
/// This function assumes that sufficient length exists in the packet
/// (>= 34 bytes, MAC+IP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns 1 (true) if the protocol is UDP, other 0 (false)
char		isUDP (char * pPacket, int nLength);

/// @brief Give a pointer to the IPv4 source address
/// @param pPacket A pointer to the beginning of the packet
/// @returns Memory address where the IPv4 source address can be found
char *	getIPv4_Address_Source (char * pPacket);

/// @brief Give a pointer to the IPv4 destination address
/// @param pPacket A pointer to the beginning of the packet
/// @returns Memory address where the IPv4 destination address can be found
char *	getIPv4_Address_Dest (char * pPacket);

/// This function assumes that sufficient length exists in the packet
/// (>= 34 bytes, MAC+IP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns 1 (true) if the protocol is TCP, other 0 (false) 
unsigned char 	getIPv4Protocol (char * pPacket);

/// @brief Retrieve the packet length in terms of IP payload
/// This function assumes that sufficient length exists in the packet
/// (>= 34 bytes, MAC+IP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns The length stated in the IPv4 length field
unsigned short	getPacketLength (char * pPacket, int nLength);


/// @brief Retrieve the packet ID from the IPv4 header
/// This function assumes that sufficient length exists in the packet
/// (>= 34 bytes, MAC+IP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns The ID stated in the IPv4 length field
unsigned short	getIPv4_ID (char * pPacket, int nLength);


/// @brief Set the IP length of the packet to a new value
void	setPacketLength (char * pPacket, unsigned short nLength);

/// @brief Set the IP ID field to a new value
/// Note that this function does not update the IP checksum
void	setIP_ID (char * pPacket, unsigned short nID);


/// @brief Retrieve the TCP sequence number
/// This function assumes that sufficient length exists in the packet
/// (>= 54 bytes, MAC+IP+TCP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns The TCP sequence number of the packet
unsigned int getTCP_SeqNumber (char * pPacket, int nLength);

/// @brief Retrieve the TCP ack number
/// This function assumes that sufficient length exists in the packet
/// (>= 54 bytes, MAC+IP+TCP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns The TCP ack number of the packet
unsigned int getTCP_AckNumber (char * pPacket, int nLength);

/// @brief Retrieve the TCP checksum
/// This function assumes that sufficient length exists in the packet
/// (>= 54 bytes, MAC+IP+TCP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns The TCP checksum of the packet
unsigned short getTCP_Checksum (char * pPacket, int nLength);


/// @brief Overwrite the TCP sequence number
/// This function assumes that sufficient length exists in the packet
/// (>= 54 bytes, MAC+IP+TCP) with the Layer 2 header being Ethernet
/// @param nSeqNum The new TCP sequence number to include
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
void writeTCP_SeqNumber (unsigned int nSeqNum, char * pPacket, int nLength);

/// @brief Overwrite the TCP ack number
/// This function assumes that sufficient length exists in the packet
/// (>= 54 bytes, MAC+IP+TCP) with the Layer 2 header being Ethernet
/// @param nAckNum The new TCP ack number to include
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
void writeTCP_AckNumber (unsigned int nAckNum, char * pPacket, int nLength);


/// Dump a character array in hex separated by the specified character
/// @param pData The array to dump to stdout
/// @param bySeparator The separation character
/// @param nLength The length of the array 
void dumpHex (char * pData, char bySeparator, int nLength);

/// Dump an IPv4 address to stdout
/// @param pData A pointer to where the IPv4 address is located
void dumpIPv4 (char * pData);

/// Convert an IPv4 address to a string
/// @param pData A pointer to where the IPv4 address is located
string IPv4toString (char * pData);

/// Dump the internal contents of an ARP packet
void dumpParseARP (char * pData);

void dumpIP_brief (char * pData);


void dumpTCP_brief (char * pData);


unsigned short	getTCP_SrcPort (char * pPacket);

unsigned short	getTCP_DestPort (char * pPacket);

/// @brief Return the TCP header size in bytes
/// Access the TCP header to determine the TCP header size.  Note that the size
/// returned is in actual bytes, not 32-bit words
/// This function assumes that sufficient length exists in the packet
/// (>= 54 bytes, MAC+IP+TCP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns The length of the TCP header in bytes
unsigned	char getTCP_HeaderSize (char * pData, int nLength);

void setTCPFlag_PSH (char * pData, int nLength, char bVal);
char getTCPFlag_PSH (char * pData, int nLength);


void setTCPFlag_ACK (char * pData, int nLength, char bVal);

/// @brief Determine if the ACK flag is set in the TCP header
/// Use a bitmask to determine if the ACK flag is set.  Note that the result is a
/// boolean true/false (non-zero or zero).  Do not test to see if the result is 1.
/// This function assumes that sufficient length exists in the packet
/// (>= 54 bytes, MAC+IP+TCP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns Non-zero if the flag is set, 0 otherwise
char getTCPFlag_ACK (char * pData, int nLength);

/// @brief Determine if the FIN flag is set in the TCP header
/// Use a bitmask to determine if the FIN flag is set.  Note that the result is a
/// boolean true/false (non-zero or zero).  Do not test to see if the result is 1.
/// This function assumes that sufficient length exists in the packet
/// (>= 54 bytes, MAC+IP+TCP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns Non-zero if the flag is set, 0 otherwise
char getTCPFlag_FIN (char * pData, int nLength);

/// @brief Determine if the SYN flag is set in the TCP header
/// Use a bitmask to determine if the SYN flag is set.  Note that the result is a
/// boolean true/false (non-zero or zero).  Do not test to see if the result is 1.
/// This function assumes that sufficient length exists in the packet
/// (>= 54 bytes, MAC+IP+TCP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns Non-zero if the flag is set, 0 otherwise
char getTCPFlag_SYN (char * pData, int nLength);

/// @brief Determine if the RST flag is set in the TCP header
/// Use a bitmask to determine if the SYN flag is set.  Note that the result is a
/// boolean true/false (non-zero or zero).  Do not test to see if the result is 1.
/// This function assumes that sufficient length exists in the packet
/// (>= 54 bytes, MAC+IP+TCP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns Non-zero if the flag is set, 0 otherwise
char getTCPFlag_RST (char * pData, int nLength);

/// @brief Determine if the packet has IP options in the IPv4 header
/// This function assumes that sufficient length exists in the packet
/// (>= 54 bytes, MAC+IP+TCP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns 1 if the packet has options, 0 if it does not
char hasIPOptions (char * pData, int nLength);

/// @brief Retrieve the IP header size in actual bytes
/// Note that the size
/// returned is in actual bytes, not 32-bit words.
/// This function assumes that sufficient length exists in the packet
/// (>= 54 bytes, MAC+IP+TCP) with the Layer 2 header being Ethernet
/// @param pPacket A pointer to the beginning of the packet
/// @param nLength The length of the packet
/// @returns The IPv4 header size in bytes
unsigned char getIP_HeaderSize (char * pData, int nLength);

void	updateIPv4_Checksum (char * pData, int nLength);

unsigned short ip_sum_calc (unsigned short len_ip_header, unsigned short * pBuff);

void	updateTCP_Checksum (char * pData, int nLength);

/** @brief Retrieve the TCP payload size
 * Given a TCP packet, calculate the size of the data payload
 * carried by TCP.  This function assumes that the given data
 * pointer is a pointer to the beginning of the captured payload
 * starting at the Ethernet data 
 * @param pData Start of data
 * @returns The length of the data in bytes in the actual data payload */
unsigned short getTCP_PayloadSize (char * pData);


unsigned short tcp_sum_calc(unsigned short len_tcp, unsigned short src_addr [], unsigned short dest_addr[],  unsigned short buff[]);

char	isPureAck (char * pData, int nLength);

/** Compute the distance between the two timeval structs and
 * return the result in microseconds. Note that the absolute
 * difference is the value returned rather than a positive or
 * negative value
 * @param pComp1 A pointer to a valid timeval struct
 * @param pComp2 A pointer to a valid timeval struct
 * @returns Difference between the two times in microseconds
 */
unsigned int		calcTimeDiff (struct timeval * pComp1, struct timeval * pComp2);

/// @param pAdj timeval struct to adjust
/// @param nAddUS Number of microseconds to add
void				adjustTime (struct timeval * pAdj, unsigned int nAddUS);

/// @returns 1 if pComp1 is greater than or equal to pComp2, zero otherwise
char	compareTime (struct timeval * pComp1, struct timeval * pComp2);


void displayTime (struct timeval * pTime);

/// Eliminate TCP options from the header
/// They are called options for a reason :)
unsigned short	purgeTCPOptions (char * pData, int nLength);

unsigned short  getTCPOptions_Size (char * pData);

/// Note this does not recalc the checksum
/// 0 for the window size means ignore
void setTCP_WindowSize (char * pData, unsigned short nWindow);


void clearTCP_Flags (char * pData);

#endif /*IPUTILS_H_*/

