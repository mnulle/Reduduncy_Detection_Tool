/*
 * fmnc_connection_tcp_slice.cc
 *
 *  Created on: Jun 3, 2014
 *      Author: striegel
 */

#include "fmnc_connection_tcp_slice.h"
#include "../util/ip-utils.h"

#include <iostream>
using namespace std;

FMNC_Connection_TCP_Slice::FMNC_Connection_TCP_Slice ()
{
	m_nClientSeqNum = 0;
	m_nServerSeqNum = 0;
	m_nState = FMNC_TCP_STATE_NONE;
	m_lAckedNum_FromClient = 0;
	m_pRefPacket = NULL;
}

FMNC_Connection_TCP_Slice::~FMNC_Connection_TCP_Slice ()
{
	// If the reference packet is present, we own it.  We need to therefore free the packet back into the
	//  global pool
	if(m_pRefPacket != NULL)
	{
		m_pRefPacket->release();
		m_pRefPacket = NULL;
	}
}

FMNC_List_Measurement_Packets *		FMNC_Connection_TCP_Slice::getMeasurementPkts_Setup ()
{
	return &m_Setup;
}

FMNC_List_Measurement_Packets *		FMNC_Connection_TCP_Slice::getMeasurementPkts_Received ()
{
	return &m_Rcvd;
}

FMNC_List_Measurement_Packets *		FMNC_Connection_TCP_Slice::getMeasurementPkts_Sent ()
{
	return &m_Sent;
}


void FMNC_Connection_TCP_Slice::setAckedNum_FromClient 		(uint32_t lSeqNum)
{
	m_lAckedNum_FromClient = lSeqNum;
}

uint32_t FMNC_Connection_TCP_Slice::getAckedNum_FromClient 	()
{
	return m_lAckedNum_FromClient;
}


int	 FMNC_Connection_TCP_Slice::getState ()
{
	return m_nState;
}

void FMNC_Connection_TCP_Slice::setState (int nState)
{
	m_nState = nState;
}

bool FMNC_Connection_TCP_Slice::isMatch (Packet * pPacket)
{
	// Compare source / destination IP addresses

	if(memcmp(pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_SRCIP, getAddress_Source()->getAddress(), DEFAULT_IP_LEN) != 0)
	{
		return false;
	}

	if(memcmp(pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_DSTIP, getAddress_Dest()->getAddress(), DEFAULT_IP_LEN) != 0)
	{
		return false;
	}

	uint16_t	nPort;

	nPort = getPort_Source();

	if(nPort == getTCP_SrcPort(pPacket->getData()))
	{
		return false;
	}

	nPort = getPort_Dest();

	if(nPort == getTCP_DestPort(pPacket->getData()))	{
		return false;
	}

	return true;
}


void FMNC_Connection_TCP_Slice::reversePacket (Packet * pPacket)
{
	// Flip the Layer 2 MAC address
	char	byMAC[DEFAULT_MAC_LEN];

	memcpy(byMAC, pPacket->getData()+OFFSET_ETH_DSTMAC, DEFAULT_MAC_LEN);
	memcpy(pPacket->getData()+OFFSET_ETH_DSTMAC, pPacket->getData()+OFFSET_ETH_SRCMAC, DEFAULT_MAC_LEN);
	memcpy(pPacket->getData()+OFFSET_ETH_SRCMAC, byMAC, DEFAULT_MAC_LEN);

	// Reset the TTL

	// Flip the source and destination addresses
	memcpy(pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_SRCIP, getAddress_Dest()->getAddress(), DEFAULT_IP_LEN);
	memcpy(pPacket->getData()+L2_OFFSET+OFFSET_IPV4_PROTOCOL_DSTIP, getAddress_Source()->getAddress(), DEFAULT_IP_LEN);

	// TODO: Double check the size of the IPv4 header

	// Recompute the IP header?

	// Flip the source and destination ports
	uint16_t	nPort;

	// TODO: Validate endianness here, we might be in trouble??

	nPort = htons(getPort_Dest());
	memcpy(pPacket->getData()+L2_OFFSET+L3_IPV4_OFFSET+OFFSET_TCP_SRCPORT, &nPort, 2);
	nPort = htons(getPort_Source());
	memcpy(pPacket->getData()+L2_OFFSET+L3_IPV4_OFFSET+OFFSET_TCP_DSTPORT, &nPort, 2);

	// Recompute the TCP header?

	// Fix the IP checksum
	updateIPv4_Checksum(pPacket->getData(), pPacket->getLength());

	updateTCP_Checksum(pPacket->getData(), pPacket->getLength());

	updateIPv4_Checksum(pPacket->getData(), pPacket->getLength());
}

void FMNC_Connection_TCP_Slice::setReferencePacket (Packet * pPacket)
{
	m_pRefPacket = pPacket;
}

Packet * FMNC_Connection_TCP_Slice::getReferencePacket ()
{
	return m_pRefPacket;
}

Packet * FMNC_Connection_TCP_Slice::respondToSYN (Packet * pSynPacket)
{
	Packet 	*	pPacket;

	// Make a measurement packet to capture information with respect to the
	//  IP header and requested TCP options (even though we don't support any options)
	FMNC_Measurement_Packet_SYN	*	pMeasureSyn;

	pMeasureSyn = new FMNC_Measurement_Packet_SYN();
	pMeasureSyn->instrumentPacket(pSynPacket);
	m_Setup.addPacket(pMeasureSyn);

	// Clone and reverse the packet to return it to the client
	pPacket = pSynPacket->cloneFromPool();

	// Flip the source and destination aspects
	reversePacket(pPacket);

	// Clear any TCP options (we don't play TCP options)
	clearOptions_TCP(pPacket);

	// Make sure the SYN flag is still true
	//  Optional - should still be set

	// Set the ACK flag to be true
	setTCPFlag_ACK(pPacket->getData(), pPacket->getLength(), 1);

	// We ACK back at the client sequence number + 1
	incSeqNum_Client(1);
	writeTCP_AckNumber(getSeqNum_Client(), pPacket->getData(), pPacket->getLength());

	writeTCP_SeqNumber(getSeqNum_Server(), pPacket->getData(), pPacket->getLength());
	incSeqNum_Server(1);

	// Recompute the TCP checksum

	// Recompute any IP checksums
	updateIPv4_Checksum(pPacket->getData(), pPacket->getLength());
	updateTCP_Checksum(pPacket->getData(), pPacket->getLength());
	updateIPv4_Checksum(pPacket->getData(), pPacket->getLength());

	// Change the connection state to denote that we have now done a SYN-ACK
	setState(FMNC_TCP_STATE_SYNACK);

	// Create a measurement packet
	FMNC_Measurement_Packet_TCP	*	pMeasureSynAck;

	pMeasureSynAck = new FMNC_Measurement_Packet_TCP();
	pMeasureSynAck->instrumentPacket(pPacket);
	pMeasureSynAck->appendMetaData("SynAck");
	m_Setup.addPacket(pMeasureSynAck);

	// Clone / save the packet for outbound info, particularly Layer 2 info to help us out
	setReferencePacket(pPacket->cloneFromPool());

	return pPacket;
}

void FMNC_Connection_TCP_Slice::clearOptions_TCP (Packet * pPacket)
{
	uint16_t	nDiff;

	nDiff = purgeTCPOptions(pPacket->getData(), pPacket->getLength());

	if(nDiff > 0)
	{
		cout << "TCP Options Purge: " << nDiff << " bytes" << endl;
		pPacket->setLength(pPacket->getLength() - nDiff);
	}
	else
	{
		cout << "No TCP Options to purge" << endl;
		// Nothing to purge
	}
}

void FMNC_Connection_TCP_Slice::setSeqNum_Server (uint32_t lSeqNum)
{
	m_nServerSeqNum = lSeqNum;
}

uint32_t FMNC_Connection_TCP_Slice::getSeqNum_Server()
{
	return m_nServerSeqNum;
}

void FMNC_Connection_TCP_Slice::incSeqNum_Client (uint32_t lInc)
{
	m_nClientSeqNum += lInc;
}

void FMNC_Connection_TCP_Slice::setSeqNum_Client (uint32_t lSeqNum)
{
	m_nClientSeqNum = lSeqNum;
}

uint32_t FMNC_Connection_TCP_Slice::getSeqNum_Client()
{
	return m_nClientSeqNum;
}

void FMNC_Connection_TCP_Slice::incSeqNum_Server (uint32_t lInc)
{
	m_nServerSeqNum += lInc;
}


void FMNC_Connection_TCP_Slice::populateInformation (Packet * pPacket)
{
	FMNC_Connection::populateInformation(pPacket);

	// Fill in the client sequence number as seen by this SYN packet
	setSeqNum_Client(getTCP_SeqNumber(pPacket->getData(), pPacket->getLength()));


	cout <<"  Src IP: ";
	dumpIPv4(getIPv4_Address_Source(pPacket->getData()));
	cout << endl;
	cout << "  Seq Number: " << getSeqNum_Client() << endl;
}

void FMNC_Connection_TCP_Slice::populateConnectionTuple (Packet * pPacket)
{
	NetAddressIPv4	*	pAddress;

	cout << "Populating the connection tuple" << endl;

	// Set the source and destination address tuples
	pAddress = getAddress_Source();
	pAddress->setAddress((unsigned char *) getIPv4_Address_Source(pPacket->getData()), DEFAULT_IP_LEN);

	pAddress = getAddress_Dest();
	pAddress->setAddress((unsigned char *) getIPv4_Address_Dest(pPacket->getData()), DEFAULT_IP_LEN);

	// Set the source and destination ports
	setPort_Source (getTCP_SrcPort(pPacket->getData()));
	setPort_Dest (getTCP_DestPort(pPacket->getData()));
}

