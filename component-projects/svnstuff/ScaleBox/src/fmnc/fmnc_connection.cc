/*
 * fmnc_connection.cc
 *
 *  Created on: Apr 14, 2014
 *      Author: striegel
 */

#include "fmnc_connection.h"


FMNC_Connection::FMNC_Connection ()
{
	m_pClient = NULL;
	m_pTestSequence = NULL;
}

FMNC_Client * FMNC_Connection::getClient ()
{
	return m_pClient;
}

void FMNC_Connection::setClient (FMNC_Client * pClient)
{
	m_pClient = pClient;
}

int FMNC_Connection::getType ()
{
	return m_nType;
}

void FMNC_Connection::setType (int nType)
{
	m_nType = nType;
}

timeval * FMNC_Connection::getTime_Creation ()
{
	return &m_Time_Creation;
}

void	  FMNC_Connection::setTime_Creation (timeval * pCreation)
{
	m_Time_Creation = *pCreation;
}

timeval * FMNC_Connection::getTime_LastUsed ()
{
	return &m_Time_LastUsed;
}

void	  FMNC_Connection::setTime_LastUsed (timeval * pLastUsed)
{
	m_Time_LastUsed = *pLastUsed;
}

NetAddressIPv4	* FMNC_Connection::getAddress_Source ()
{
	return &m_IP_Source;
}

NetAddressIPv4 	* FMNC_Connection::getAddress_Dest ()
{
	return &m_IP_Dest;
}

uint16_t		  FMNC_Connection::getPort_Source ()
{
	return m_nPort_Source;
}

void			  FMNC_Connection::setPort_Source (uint16_t nPort)
{
	m_nPort_Source = nPort;
}

uint16_t		  FMNC_Connection::getPort_Dest ()
{
	return m_nPort_Dest;
}

void			FMNC_Connection::setPort_Dest (uint16_t nPort)
{
	m_nPort_Dest = nPort;
}

void FMNC_Connection::setTestSequence (FMNC_Test_Sequence * pTestSeq)
{
	m_pTestSequence = pTestSeq;
}

FMNC_Test_Sequence * FMNC_Connection::getTestSequence ()
{
	return m_pTestSequence;
}

void FMNC_Connection::displayTuple ()
{
	cout << "  Src: ";
	cout << m_IP_Source.toString();
	cout << ":";
	cout << m_nPort_Source;
	cout << "  Dst: ";
	cout << m_IP_Dest.toString();
	cout << ":";
	cout << m_nPort_Dest;
}


void	FMNC_Connection::populateConnectionTuple (Packet * pPacket)
{

}

void FMNC_Connection::populateInformation (Packet * pPacket)
{
	// Mark this connection as being created effectively when the packet arrived
	setTime_Creation(pPacket->getArrivalTime());

	// Mark the connection as being "accessed" when it was created (for garbage collection)
	setTime_LastUsed(pPacket->getArrivalTime());


	populateConnectionTuple(pPacket);
}

bool FMNC_Connection::isMatch (Packet * pPacket)
{
	return false;
}

FMNC_List_Connections::FMNC_List_Connections ()
{

}

bool FMNC_List_Connections::add (FMNC_Connection * pConnection)
{
	m_Connections.push_back(pConnection);

	return true;
}

FMNC_Connection *	FMNC_List_Connections::findConnection (Packet * pPacket)
{
	int		j;

	// TODO: Thread safety?
	for(j=0; j<m_Connections.size(); j++)
	{
		if(m_Connections[j]->isMatch(pPacket))
		{
			return m_Connections[j];
		}
	}

	return NULL;
}

void FMNC_List_Connections::dumpList ()
{
	int		j;

	cout << "Connection List of " << m_Connections.size();

	for(j=0; j<m_Connections.size(); j++)
	{
		cout << "  C ";
		printf("%03d", j);
		cout << " : ";

		m_Connections[j]->displayTuple();
	}
}

