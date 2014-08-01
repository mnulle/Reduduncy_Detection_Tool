/*
 * fmnc_measurement_packet.h
 *
 *  Created on: Jun 9, 2014
 *      Author: Aaron
 */

#ifndef FMNC_MEASUREMENT_PACKET_H_
#define FMNC_MEASUREMENT_PACKET_H_

// Measurement packets for FMNC
#include <vector>
using namespace std;

#include <string>
using namespace std;

#include "../pkt/Packet.h"

/// Base class for one measurement packet
class FMNC_Measurement_Packet
{
	public:
		FMNC_Measurement_Packet();

		void 	setime (struct timeval * pTime);
		struct timeval * 	getTime ();

		virtual bool	instrumentPacket (Packet * pPacket);

		bool instrumentPacket_Base ();

		virtual string 	getXML ();

		string getXML_Attributes_Base ();

		/// Retrieve the time value as a string along with the attribute identifier
		string	getAttribute_Time ();

		string 	getAttribute_Time (timeval * pBaseTime);
		string 	getAttribute_Time (FMNC_Measurement_Packet * pBasePacket);

		string	getAttribute_MetaData ();

		/// Retrieve the time value as a string
		string 	getTime_AsString ();

		void	setMetaData (string sData);
		void	appendMetaData (string sAppend);
		string 	getMetaData ();

	private:
		timeval 	m_Time;

		string		m_sMetaData;
};

/// List of measurement packets
class FMNC_List_Measurement_Packets
{
	public:
		FMNC_List_Measurement_Packets ();
		~FMNC_List_Measurement_Packets ();

		string getXML(string sTag);

		void 	addPacket (FMNC_Measurement_Packet * pPacket);

	private:
		vector<FMNC_Measurement_Packet *>	m_Packets;
};

/// An IPv4 measurement packet with the relevant IPv4 fields recorded
class FMNC_Measurement_Packet_IPv4 : public FMNC_Measurement_Packet
{
	public:
		FMNC_Measurement_Packet_IPv4 ();

		void	setTTL (uint8_t byTTL);
		uint8_t	getTTL ();

		void	setTOS (uint8_t byTOS);
		uint8_t	getTOS ();

		void	setLength (uint16_t nLength);
		uint16_t	getLength ();

		void 	setID (uint16_t nID);
		uint16_t	getID ();

		virtual bool instrumentPacket (Packet * pPacket);

		virtual string 	getXML ();

		virtual string getXML_Attributes_IPv4 ();

	protected:
		bool instrumentPacket_IPv4 (Packet * pPacket);

	private:
		// TTL field
		uint8_t		m_byTTL;

		// Type of Service field
		uint8_t		m_byTOS;

		// Length of the packet (as viewed from the IP header)
		uint16_t	m_nLength;

		// The ID of the packet
		uint16_t	m_nID;
};


/// Instrumentation for an inbound SYN packet.  Whereas we do not expect options on
/// subsequent packets, it is worthwhile to note any and all options requested during
/// the initial SYN packet from the client with regards to SACK or other aspects
class FMNC_Measurement_Packet_SYN : public FMNC_Measurement_Packet_IPv4
{
	public:
		FMNC_Measurement_Packet_SYN ();


		virtual bool instrumentPacket (Packet * pPacket);

		void	setOptionLength (uint8_t byLength);
		uint8_t	getOptionLength ();

		virtual string 	getXML ();

		void	setClientSeqNum (uint32_t lSeqNum);
		uint32_t	getClientSeqNum ();

	private:
		// Mirror of the TCP options field (if present)
		uint8_t  	m_byOptions[100];
		uint8_t		m_byOptionLength;

		uint32_t 	m_lClientSeq;

};

/// A simple acknowledgment packet
class FMNC_Measurement_Packet_TCP : public FMNC_Measurement_Packet_IPv4
{
	public:
		FMNC_Measurement_Packet_TCP ();

		virtual string 	getXML ();

		void	setSeqNum (uint32_t lSeqNum);
		uint32_t	getClientSeqNum ();

		void	setAckNum (uint32_t lSeqNum);
		uint32_t	getAckNum ();

		virtual bool instrumentPacket (Packet * pPacket);

	private:
		bool		m_bFlagAck;
		uint32_t 	m_lSeqNum;
		uint32_t	m_lAckNum;
};



#endif /* FMNC_MEASUREMENT_PACKET_H_ */
