/*
 * fmnc_manager.cc
 *
 *  Created on: Apr 1, 2014
 *      Author: striegel
 */

#include "fmnc_manager.h"
#include "fmnc_connection_tcp_slice.h"

#include "../mon/Thread_Archive.h"
#include "../mon/Thread_Timer.h"

#include "../xml/NodeDOM.h"
#include "../xml/NodeElem.h"

#include "../util/ip-utils.h"

#include "fmnc_test_sequence.h"
#include "fmnc_measurement_packet.h"

/////////////////////////////////////////////////////////////////////////

FMNC_ManagerStats::FMNC_ManagerStats () : Stats()
{
	  Allocate(FMNC_MANAGER_STAT_LAST);
}

/////////////////////////////////////////////////////////////////////////

void	FMNC_ManagerStats::Get_Title	(int nStat, char * szTitle)
{
	switch(nStat)
	{
		default:
			sprintf(szTitle, "F%d", nStat);
			break;
	}
}



FMNC_Manager::FMNC_Manager ()
{
	// Initialize appropriate values
}

char FMNC_Manager::initialize ()
{
	return 1;
}

bool FMNC_Manager::extractExtendedDOM (NodeDOM * pNode) {
	return true;
}

bool FMNC_Manager::processTimer (TimerEvent * pEvent)
{
	cout << "*** FMNC: Processing a timer of value " << pEvent->getTimerID() << endl;

	switch(pEvent->getTimerID())
	{
		case FMNC_TIMER_START_SLICING:
			return doTimer_StartSlicing(pEvent);
		case FMNC_TIMER_DO_SLICING:
			return doTimer_Slicing(pEvent);
	}
	return true;
}

bool FMNC_Manager::doTimer_StartSlicing (TimerEvent * pEvent)
{
	FMNC_Connection_TCP_Slice *	pSlicedConnection;

	// We know the data payload is the connection that we are associated with
	pSlicedConnection = (FMNC_Connection_TCP_Slice *) pEvent->getData();

	cout << "  Starting the slicing process" << endl;

	// Create a test sequence
	FMNC_Test_Sequence	*	pTestSequence;

	pTestSequence = new FMNC_Test_Sequence();
	pTestSequence->setConnection(pSlicedConnection);

	// TODO: Replace this number with #defines
	pTestSequence->populatePredefined(0);

	// Adjust the sequence numbers
	pTestSequence->adjustSeqNumber(pSlicedConnection->getSeqNum_Server());

	// Adjust the dispatch time to be based off of the current time
	timeval 	currentTime;
	gettimeofday(&currentTime, NULL);
	pTestSequence->adjustDispatchTime(&currentTime);

	// The connection is "hot" now for data
	pSlicedConnection->setState(FMNC_TCP_STATE_ACTIVE);

	// Adjust the packet timing
	TimerEvent 	*	pFirstTimer;

	pFirstTimer = TimerEvent::createTimer();
	pFirstTimer->setTimerID(FMNC_TIMER_DO_SLICING);
	pFirstTimer->setInterval_ms(0);
	pFirstTimer->armTimer();

	if(!addTimerEvent(pFirstTimer))
	{
		cerr << "* Error: Unable to add timer" << endl;
		pFirstTimer->release();
		return false;
	}

	return true;
}

bool FMNC_Manager::doTimer_Slicing (TimerEvent * pEvent)
{
	FMNC_Connection_TCP_Slice *	pSlicedConnection;

	// Create a test sequence
	FMNC_Test_Sequence	*	pTestSequence;

	pTestSequence = (FMNC_Test_Sequence *) pEvent->getData();

	pSlicedConnection = (FMNC_Connection_TCP_Slice *) pTestSequence->getConnection();

	cout << "  Continuing the slicing process" << endl;


	// Make a new packet pre-populated from our reference packet.  The reference packet is
	//  saved since we need to track what the inbound / outbound L2 headers are.  We also save
	//  Layer 3 to help us out as well

	Packet 	*	pSlicePacket;

	pSlicePacket = pSlicedConnection->getReferencePacket()->cloneFromPool();

	pTestSequence->populatePacketWithNextSend(pSlicePacket);

	// TODO: Does timer processing need the timer released?
	pSlicePacket->setArrivalTime(pEvent->getInvocationTime());
	pSlicePacket->registerWriteCallback(this, NULL);

	FMNC_Measurement_Packet_TCP	*	pMeasureData;

	pMeasureData = new FMNC_Measurement_Packet_TCP();
	pMeasureData->instrumentPacket(pSlicePacket);

	// Do we add the sequence number to the mix in the metadata?
	pSlicedConnection->getMeasurementPkts_Sent()->addPacket(pMeasureData);

	// Write that packet out the adapter
	if(!writeBackViaTap(FMNC_TAP_WAN, pSlicePacket))
	{
		// Ruh roh!
		cerr << "* Error: Failed to write back via the tap" << endl;
		pSlicePacket->release();
	}


	if(!pTestSequence->isDone())
	{
		TimerEvent 	*	pNextTimer;

		pNextTimer = TimerEvent::createTimer();
		pNextTimer->setTimerID(FMNC_TIMER_DO_SLICING);
		pNextTimer->setInterval_ms(1);
		pNextTimer->armTimer();

		if(!addTimerEvent(pNextTimer))
		{
			cerr << "* Error: Unable to add the timer for the next iteration for the test sequence" << endl;
			pNextTimer->release();
			return false;
		}
	}
	else
	{
		// Set the state that we are waiting for packets
		pSlicedConnection->setState(FMNC_TCP_STATE_WAITING);
	}

	return true;
}

NodeDOM * FMNC_Manager::getStats (NodeDOM * pRoot)
{
	return NULL;
}

CommandResult FMNC_Manager::processExtCommand (const vector<string> & theCommands, int nOffset)
{
	if(theCommands.size() <= nOffset) {
		return PROCESS_CMD_SYNTAX;
	}

//	cout << " Processing a command at the TWiCE module" << endl;

	if(theCommands[nOffset] == "help") {

		cout << " FMNC Module Help" << endl;

		return PROCESS_CMD_SUCCESS;
	}

	return PROCESS_CMD_UNKNOWN;
}

bool FMNC_Manager::processWriteCallback (Packet * pPacket, void * pData) {
	cout << " * Processing a write callback" << endl;
	return 1;
}


char FMNC_Manager::processTapPacket (Packet * pPacket, int nID)
{
	switch(nID)
    {
    	case FMNC_TAP_WAN:
    		return processPacket_WAN(pPacket);
    }

	return 0;
}

int FMNC_Manager::mapNameToID (string sName)
{

  if(sName == "WAN")
    {
      return FMNC_TAP_WAN;
    }
  else
    {
      return FMNC_TAP_NONE;
    }
}

char FMNC_Manager::isNewConnection (Packet * pPacket)
{
	//
	return 1;
}

char FMNC_Manager::mapConnectionTypeFromPort (Packet * pPacket)
{
	// Get the inbound port number (Destination)
	unsigned short	nDestPort;

	nDestPort = getTCP_DestPort(pPacket->getData());

	if(nDestPort == getPort_NI())
	{
		return FMNC_CONNTYPE_NI;
	}
	else if(nDestPort == getPort_Web())
	{
		return FMNC_CONNTYPE_WEB;
	}
	else if(nDestPort == getPort_XML())
	{
		return FMNC_CONNTYPE_XML;
	}
	else
	{
		return FMNC_CONNTYPE_UNKNOWN;;
	}
}

char FMNC_Manager::shouldAcceptNewConnection (char byType)
{
	return 1;
}

char FMNC_Manager::handleTCP_Existing (FMNC_Connection * pConnection, Packet * pPacket)
{
	// What state are we in?
	FMNC_Connection_TCP_Slice *	pSlicedConnection;
	int		nPayloadSize;

	pSlicedConnection = (FMNC_Connection_TCP_Slice * ) pConnection;

	if(getTCPFlag_SYN(pPacket->getData(), pPacket->getLength()))
	{
		cerr << "* Error: Unexpected SYN later than expected" << endl;
		return PROCESSPKT_RESULT_CONTINUE;
	}

	cout << "* TCP-EXISTING: Handling existing TCP connection" << endl;


	nPayloadSize = getTCP_PayloadSize(pPacket->getData());

	switch(pSlicedConnection->getState())
	{
		case FMNC_TCP_STATE_SYNACK:
			// Looking for a zero length TCP ACK of our ACK number
			if(getTCPFlag_ACK(pPacket->getData(), pPacket->getLength()))
			{
				// The client is now active and engaged
				pSlicedConnection->setState(FMNC_TCP_STATE_ACTIVE);

				// Did they send us data with the ACK?
				if(nPayloadSize == 0)
				{
					// This is what we are looking for

				}
				else
				{
					// TODO: Deal with it when the first ACK has a payload
				}

				// Time to do some sending / engagement of the timer
				TimerEvent 	*	pTimer;

				pTimer = TimerEvent::createTimer();
				pTimer->setTimerID(FMNC_TIMER_START_SLICING);
				pTimer->setData(pSlicedConnection);

				// How long until we start things?

				// We are doing 10 ms for now, can change that later
				//  TODO: Make this configurable

				pTimer->setInterval_ms(10);
				pTimer->repeatInterval();

				if(!addTimerEvent(pTimer))
				{
					cerr << "* Error: Unable to add timer" << endl;
					pTimer->release();
					return PROCESSPKT_RESULT_CONTINUE;
				}
			}
			else
			{
				// TODO: Process error rather than continuing to gobble
				cerr << "* Error: Client did not ACK after the SYN-ACK" << endl;
				return PROCESSPKT_RESULT_CONTINUE;
			}
			break;

		case FMNC_TCP_STATE_ACTIVE:
			// Mark it and gobble the packet

			// Instrument the received packet
			FMNC_Measurement_Packet_TCP	*	pMeasureAck;

			pMeasureAck = new FMNC_Measurement_Packet_TCP();
			pMeasureAck->instrumentPacket(pPacket);

			pSlicedConnection->getMeasurementPkts_Received()->addPacket(pMeasureAck);

			// We are eating the packet (none shall pass)
			pPacket->releasePacket();
			return PROCESSPKT_RESULT_CONSUMED;

		default:
			return PROCESSPKT_RESULT_CONTINUE;
	}

	return PROCESSPKT_RESULT_CONTINUE;
}

char FMNC_Manager::handleTCP (Packet * pPacket)
{
	FMNC_Connection * pConnection;

	// Have we seen this connection (flow) before?
	pConnection = m_Connections.findConnection(pPacket);

	if(pConnection != NULL)
	{
		cout << "** handleTCP: Existing TCP connection" << endl;
		return handleTCP_Existing(pConnection, pPacket);
	}
	else
	{
		cout << "** handleTCP: Unknown TCP connection" << endl;
	}

	// We have not seen the connection before.  The only kind we might think about is a SYN
	//  connection initiating a new connection

	// Is this a new connection (SYN is set)?
	if(getTCPFlag_SYN(pPacket->getData(), pPacket->getLength()))
	{
		m_Stats.Add_Stat(FMNC_MANAGER_COUNT_SYN, 1);

		// Any additional filtering (SYN/FIN is set)?
		// TODO: Check extra filtering

		// At the SYN stage, we do not know who this is client-side due to various NAT
		// issues.  We just know this is likely a new connection


		char		byMode;

		byMode = mapConnectionTypeFromPort(pPacket);

		if(shouldAcceptNewConnection(byMode))
		{
			// Set up a new connection which is a TCP slicer on the outbound (for now)
			FMNC_Connection_TCP_Slice	*	pConnection;

			pConnection = new FMNC_Connection_TCP_Slice();

			// Ask the connection object to populate the info from the packet
			pConnection->populateInformation(pPacket);

			// Set the TCP outbound sequence number (server-side).  We need to do it at the manager
			//  level depending on how sophisticated we want to be in case we want to separate out
			//  different sequence numbers on the response to prevent spoofing
			pConnection->setSeqNum_Server(getSequenceNumber(pPacket));

			// Add it to the mix
			m_Connections.add(pConnection);

			// Ask the connection object to help us draft a response
			Packet	*	pResponse;

			pResponse = pConnection->respondToSYN (pPacket);

			cout << "  New SYN recognized" << endl;

			// Did we get a good value?  Always check the results to make sure they are a minimum at
			//  least non-NULL
			if(pResponse == NULL)
			{
				cerr << "Error: Packet response after the SYN was NULL" << endl;
				cerr << "  handleTCP - fmnc_manager.cc" << endl;

				// Let the packet go
				return PROCESSPKT_RESULT_CONTINUE;
			}
			else
			{
//				cout << "Attempting to write packet back to the potential client" << endl;

				pResponse->registerWriteCallback(this, NULL);

//				cout << " Original packet" << endl;
//				pPacket->showHex(0, pPacket->getLength());
//				cout << endl;

//				cout << " Response packet" << endl;
//				pResponse->showHex(0, pResponse->getLength());
//				cout << endl;

				// Write that packet out the adapter
				if(!writeBackViaTap(FMNC_TAP_WAN, pResponse))
				{
					// Ruh roh!
					cerr << "* Error: Failed to write back via the tap" << endl;
					pResponse->release();
				}

				// Eat the packet noting that it is ours
				pPacket->release();
				return PROCESSPKT_RESULT_CONSUMED;
			}
		}
		else
		{
			// Cut it out
			return PROCESSPKT_RESULT_CONTINUE;
		}
	}
	else
	{
		cout << "** handleTCP: Unknown TCP connection - SYN flag not set - ignoring" << endl;
	}

	return PROCESSPKT_RESULT_CONTINUE;
}

uint32_t	FMNC_Manager::getSequenceNumber (Packet * pOutbound)
{
	return 1000000;
}

char FMNC_Manager::handleUDP (Packet * pPacket)
{
	return PROCESSPKT_RESULT_CONTINUE;
}

char FMNC_Manager::processPacket_WAN (Packet * pPacket)
{
//	cout << "Got a packet of length " << pPacket->getLength() << endl;

	// Increment our packet count
	m_Stats.Add_Stat(FMNC_MANAGER_TOTAL_COUNT_PKTS, 1);
	m_Stats.Add_Stat(FMNC_MANAGER_TOTAL_VOLUME_PKTS, pPacket->getLength());

	// Is this packet even an IP packet?
	if(!isPacketIP(pPacket->getData(), pPacket->getLength()))
	{
		m_Stats.Add_Stat(FMNC_MANAGER_NONIP_COUNT_PKTS, 1);
		m_Stats.Add_Stat(FMNC_MANAGER_NONIP_VOLUME_PKTS, 1);
		return PROCESSPKT_RESULT_CONTINUE;
	}

	// Is this IPv4, IPv6, or who knows what?
	switch(getIPVersion(pPacket->getData(), pPacket->getLength()))
	{
		case 4:
			break;
		case 6:
			m_Stats.Add_Stat(FMNC_MANAGER_IPV6_COUNT_PKTS, 1);
			m_Stats.Add_Stat(FMNC_MANAGER_IPV6_VOLUME_PKTS, 1);
			return PROCESSPKT_RESULT_CONTINUE;

		default:
			break;
	}

	// Is this packet TCP or UDP?
	if(isTCP(pPacket->getData(), pPacket->getLength()))
	{
		return handleTCP(pPacket);
	}
	else if(isUDP(pPacket->getData(), pPacket->getLength()))
	{
		return handleUDP(pPacket);
	}
	else
	{
		// IPsec or something but we don't know what it is so ignore it but count it
	}

	return 0;
}

uint16_t	FMNC_Manager::getPort_NI ()
{
	return m_nPort_NI;
}

void 		FMNC_Manager::setPort_NI (uint16_t nPort)
{
	m_nPort_NI = nPort;
}

uint16_t	FMNC_Manager::getPort_Web ()
{
	return m_nPort_Web;
}

void		FMNC_Manager::setPort_Web (uint16_t nPort)
{
	m_nPort_Web = nPort;
}

uint16_t	FMNC_Manager::getPort_XML ()
{
	return m_nPort_XML;
}

void		FMNC_Manager::setPort_XML (uint16_t nPort)
{
	m_nPort_XML = nPort;
}

