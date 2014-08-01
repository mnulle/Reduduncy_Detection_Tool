/*
 * fmnc_connection.h
 *
 *  Created on: Apr 14, 2014
 *      Author: striegel
 */

#ifndef FMNC_CONNECTION_H_
#define FMNC_CONNECTION_H_

#include "../pkt/Packet.h"
#include "../pkt/address/NetAddressIPv4.h"

#include <vector>
using namespace std;


class FMNC_Client;
class FMNC_Test_Sequence;

/** One individual Fast Mobile Network Connection that is captured by a five-tuple
 * designation which includes source IP, destination IP, source port, destination
 * port, and protocol (TCP, UDP).
 *
 */
class FMNC_Connection
{
	public:
		FMNC_Connection ();

		virtual bool	isMatch (Packet * pPacket);


		FMNC_Client * getClient ();
		void setClient (FMNC_Client * pClient);

		int getType ();
		void setType (int nType);

		timeval * getTime_Creation ();
		void	  setTime_Creation (timeval * pCreation);

		timeval * getTime_LastUsed ();
		void	  setTime_LastUsed (timeval * pLastUsed);

		uint32_t	getBytes_Sent ();
		void		setBytes_Sent (uint32_t lBytesSent);
		void		adjustBytes_Sent (uint32_t lAdjust);

		uint32_t	getBytes_Received();
		void		setBytes_Received (uint32_t lBytesReceived);
		void		adjustBytes_Received (uint32_t lAdjust);

		uint32_t	getPackets_Sent ();
		void		setPackets_Sent (uint32_t lPacketSent);
		void		adjustPackets_Sent (uint32_t lAdjust);

		uint32_t	getPackets_Received    ();
		void		setPackets_Received    (uint32_t lPacketsReceived);
		void		adjustPackets_Received (uint32_t lAdjust);

		virtual void	populateInformation (Packet * pPacket);

		virtual void	populateConnectionTuple (Packet * pPacket);

		/// Display the connection tuple info the console in a single line, no end carriage return
		void			displayTuple ();

		NetAddressIPv4	* getAddress_Source ();
		NetAddressIPv4 	* getAddress_Dest ();
		uint16_t		  getPort_Source ();
		void				setPort_Source (uint16_t nPort);
		uint16_t		getPort_Dest ();
		void				setPort_Dest (uint16_t nPort);

		void			setTestSequence (FMNC_Test_Sequence * pTestSeq);
		FMNC_Test_Sequence * getTestSequence ();

	private:
		/* The client associated with this particular connection */
		FMNC_Client 	*	m_pClient;

		/* What kind of connection is this particular connection? */
		int					m_nType;

		/* When was this connection created? */
		timeval				m_Time_Creation;

		/* When was this connection last used? */
		timeval				m_Time_LastUsed;

		/* How many bytes have been sent? */
		uint32_t			m_lBytes_Sent;

		/* How many bytes have been received? */
		uint32_t			m_lBytes_Received;

		/* How many packets have been sent? */
		uint32_t			m_lPkts_Sent;

		/* How many packets have been received */
		uint32_t			m_lPkts_Rcvd;

		/* Connection tuple info (IPv4 - for now) */

		// TODO: Replumb this for a generic flow tuple (v4 or v6 compliant)
		NetAddressIPv4		m_IP_Source;
		NetAddressIPv4		m_IP_Dest;
		uint16_t			m_nPort_Source;
		uint16_t			m_nPort_Dest;

		FMNC_Test_Sequence 	* m_pTestSequence;
};

class FMNC_List_Connections
{
	public:
		FMNC_List_Connections ();

		// Given an existing packet, figure out if we have this connection already
		//  in the list
		FMNC_Connection *	findConnection (Packet * pPacket);

		// Add this connection to the list of connections
		bool	add (FMNC_Connection * pConnection);

		/// Display the list of currently active connections contained within this particular list
		/// to the console
		void	dumpList ();

	private:
		vector<FMNC_Connection *>		m_Connections;
};

#endif /* FMNC_CONNECTION_H_ */
