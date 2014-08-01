/*
 * fmnc_connection_tcp_slice.h
 *
 *  Created on: Apr 14, 2014
 *      Author: striegel
 */

#ifndef FMNC_CONNECTION_TCP_SLICE_H_
#define FMNC_CONNECTION_TCP_SLICE_H_

#include <stdint.h>

#include "fmnc_connection.h"
#include "../pkt/packet.h"

#define FMNC_TCP_STATE_NONE		0		// Nothing - no state
#define FMNC_TCP_STATE_SYN		1		// SYN Received from client
#define FMNC_TCP_STATE_SYNACK	2		// SYN-ACK has been sent to client, waiting for ACK
#define FMNC_TCP_STATE_ACTIVE	3		// ACK of server side (us), normal operations
#define FMNC_TCP_STATE_WAITING	4		// Waiting for ACKs from client




/** A connection that will be sliced on the outbound writes from the server to the client
 */
class FMNC_Connection_TCP_Slice : public FMNC_Connection
{
	public:
		FMNC_Connection_TCP_Slice();
		~FMNC_Connection_TCP_Slice ();

		bool	isMatch (Packet * pPacket);

		void setSeqNum_Client (uint32_t lSeqNum);
		uint32_t getSeqNum_Client ();
		void 	 incSeqNum_Client (uint32_t lInc);

		// What has the client acked back to us, i.e. what byte range has the client successfully
		//  received and acknowledged to us as the server?
		void setAckedNum_FromClient 		(uint32_t lSeqNum);
		uint32_t getAckedNum_FromClient 	();



		void setSeqNum_Server (uint32_t lSeqNum);
		uint32_t getSeqNum_Server();

		/// @brief Increase the sequence number for the server side
		/// @param lInc The amount to increase the server sequence number
		void 	 incSeqNum_Server (uint32_t lInc);

		/// @brief Retrieve the state associated with this particular connection
		/// @returns Integer denoting the state, see the definition header for info
		int	 getState ();

		/// @brief Set the current state of the TCP connection (None, SYN, SYN+ACK)
		/// @param nState The new state for the connection
		void setState (int nState);

		// Process a packet from the client
		bool processPacketFromClient (Packet * pPacket);

		// Respond to the SYN packet and craft a response
		Packet * respondToSYN (Packet * pSynPacket);

		virtual void	populateInformation (Packet * pPacket);

		virtual void	populateConnectionTuple (Packet * pPacket);

		// Take a packet and flip all source / destination aspects including
		// Layer 2 (MAC Src, Dst), Layer 3 (Src, Dst IP), Layer 4 (Src, Dst Port)
		void	reversePacket (Packet * pPacket);

		void	clearOptions_TCP (Packet * pPacket);

		void	setReferencePacket (Packet * pPacket);
		Packet * getReferencePacket ();

		FMNC_List_Measurement_Packets *		getMeasurementPkts_Setup ();
		FMNC_List_Measurement_Packets *		getMeasurementPkts_Received ();
		FMNC_List_Measurement_Packets *		getMeasurementPkts_Sent ();

	protected:
		bool processPacketFromClient_InitialSYN (Packet * pPacket);

	private:
		// What is the current state of the connection?
		int				m_nState;

		// What is the client sequence number?
		uint32_t		m_nClientSeqNum;

		// What is the server sequence number (us)?
		uint32_t		m_nServerSeqNum;

		// What is the highest number that they have acked to us?
		uint32_t		m_lAckedNum_FromClient;

		// When did we send data out?

		Packet *		m_pRefPacket;


		FMNC_List_Measurement_Packets	m_Setup;
		FMNC_List_Measurement_Packets	m_Sent;
		FMNC_List_Measurement_Packets 	m_Received;
};


#endif /* FMNC_CONNECTION_TCP_SLICE_H_ */
