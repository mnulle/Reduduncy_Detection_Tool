/*
 * fmnc_manager.h
 *
 *  Created on: Apr 1, 2014
 *      Author: striegel
 */

#ifndef FMNC_MANAGER_H_
#define FMNC_MANAGER_H_

#include <vector>
using namespace std;

#include <string>
using namespace std;

#include <fstream>
using namespace std;

#include "../mem/MemoryPool.h"
#include "../pkt/MultiIFModule.h"
#include "../stat/Stats.h"
#include "../pkt/Packet.h"

#include "../core/Console.h"

#include "fmnc_connection.h"

#define FMNC_DEFAULT_MAX_SESSIONS		100


#define FMNC_CONNTYPE_UNKNOWN	0
#define	FMNC_CONNTYPE_NI		1
#define	FMNC_CONNTYPE_WEB		2
#define	FMNC_CONNTYPE_XML		3

#define FMNC_TIMER_START_SLICING	4
#define FMNC_TIMER_DO_SLICING		5

/** Traffic can come in only via one adapter
 */
enum eFMNC_GatewayTaps {
	FMNC_TAP_NONE=0,
	FMNC_TAP_WAN			// Traffic from the Internet side
};

/** Enumerated values for the different types of stats that the RIPPS monitor
 * will keep track of
 */
enum eFMNC_ManagerStats {
	FMNC_MANAGER_TOTAL_COUNT_PKTS,		// How many packets have we received?
	FMNC_MANAGER_TOTAL_VOLUME_PKTS,		// Packet volume

	FMNC_MANAGER_NONIP_COUNT_PKTS,		// Count of non-IP packets
	FMNC_MANAGER_NONIP_VOLUME_PKTS,		// Volume of non-IP packets

	FMNC_MANAGER_IPV6_COUNT_PKTS,
	FMNC_MANAGER_IPV6_VOLUME_PKTS,

	FMNC_MANAGER_COUNT_SYN,		// Number of received SYN packets
	FMNC_MANAGER_STAT_LAST
};

/** Statistics for the TWiCE gateway
 */
class FMNC_ManagerStats : public Stats {
	public:
		FMNC_ManagerStats ();

		/** Retrieve the title for a given field
		 * @param nStat		The index to the stat field
		 * @param szTitle		The char field to populate with the title
		 */
		virtual void		Get_Title	(int nStat, char * szTitle);


};




class FMNC_Manager : public MultiIFModule {

	public:
		FMNC_Manager();
		~FMNC_Manager ();

		/** Given the name of a tap module, map that module name to a specific
		 * ID that will be used whenever packets are seen on that particular
		 * tap. This function must be overridden to encapsulate the behavior
		 * how the names map to IDs.
		 * @param sName The name of the module
		 * @returns The ID to use for that module where -1 is a failure in name resolution
		 */
		virtual int 	mapNameToID (string sName);

		/** Process a packet from one of the underlying tap modules and
		 * appropriate process the packet to the same behavior as a processPacket
		 * call from a normal IOModule object
		 */
		virtual char	processTapPacket (Packet * pPacket, int nID);

		/** Initialize an input module by applying any necessary
		 * startup functionality such as threads, thread safety, or
		 * other initializations.
		 * @returns True if successful, false otherwise
		 */
		char		initialize ();

		/** Parse the DOM to populate the file where the DOM
		 * is generated from an XML input file. The DOM should contain
		 * the type of adapter and the underlying input and output
		 * modules. The extractExtended function takes in the
		 * extended information specific to the child class.
		 * @param pNode The base node to process for information
		 * @returns True if successful, false otherwise
		 */
		virtual bool	extractExtendedDOM (NodeDOM * pNode);

		/** Process a timer event from the associated TimerAggregator
		 * object contained within this class if timers are enabled
		 * for this particular module
		 * @param pEvent The timer event that has occurred
		 * @returns True if successfully processed, false if there was an error
		 */
		virtual bool		processTimer (TimerEvent * pEvent);

		/** Retrieve a copy of the statistics for this module
		 * @param pRoot	The DOM object serving as the root under which the stats are placed
		 * @returns A valid pointer to a NodeDOM object that serves as the root of
		 *   the statistic contents
		 */
		NodeDOM *		getStats (NodeDOM * pRoot);

		/** Process a command from the console or external control
		 * mechanism
		 * @param theCommands The vector of commands to process
		 * @param nOffset The offset at which to begin processing. The offset allows
		 *   us to pass the vector itself with modifications to the underlying
		 *   modules
		 * @returns Enumerated value representing result of command processing
		 */
		virtual CommandResult	processExtCommand (const vector<string> & theCommands, int nOffset);


		/** Process the fact that a write callback has been invoked due to a
		 * packet finally being written out due to interposed queuing delay
		 * or other mechanisms
		 * @param pPacket The packet involved in the callback
		 * @param pData The data associated with the callback
		 * @returns True if successful, false otherwise
		 */
		virtual bool	processWriteCallback (Packet * pPacket, void * pData);

		char 		processPacket_WAN (Packet * pPacket);

		uint16_t	getPort_NI ();
		void 		setPort_NI (uint16_t nPort);

		uint16_t	getPort_Web ();
		void		setPort_Web (uint16_t nPort);

		uint16_t	getPort_XML ();
		void		setPort_XML (uint16_t nPort);

		uint32_t	getMaxSessions ();
		void		setMaxSessions (uint32_t nMaxSessions);

		char mapConnectionTypeFromPort (Packet * pPacket);


		/// If we have a new connection of this particular type, should we admit it?
		/// The answer for now is always yes
		char shouldAcceptNewConnection (char byType);

		/// Is this a new connection that we have not seen before?
		char isNewConnection (Packet * pPacket);

		// Handle a TCP packet
		char handleTCP (Packet * pPacket);

		// Handle a UDP packet
		char handleUDP (Packet * pPacket);

		uint32_t	getSequenceNumber (Packet * pPacket);

		char handleTCP_Existing (FMNC_Connection * pConnection, Packet * pPacket);

	protected:

		bool 	doTimer_StartSlicing (TimerEvent * pEvent);

		bool 	doTimer_Slicing (TimerEvent * pEvent);

	private:
		// What port(s) are we listening on for non-interactive sessions?
		//   A zero signifies this port is disabled
		uint16_t		m_nPort_NI;

		// What port(s) are we listening on for web-based sessions?
		//   A zero signifies this port is disabled
		uint16_t		m_nPort_Web;

		// What port(s) are we listening on for XML / configurable sessions?
		//   A zero signifies this port is disabled
		uint16_t		m_nPort_XML;

		// Maximum number of concurrent sessions (Default = 100)
		uint32_t		m_nMax_Sessions;


		FMNC_ManagerStats	m_Stats;

		FMNC_List_Connections	m_Connections;
};

#endif /* FMNC_MANAGER_H_ */

