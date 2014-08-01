/*
 * fmnc_test_sequence.h
 *
 *  Created on: Jun 4, 2014
 *      Author: striegel
 */

#ifndef FMNC_TEST_SEQUENCE_H_
#define FMNC_TEST_SEQUENCE_H_

#include <vector>
using namespace std;

#include "fmnc_connection_tcp_slice.h"

class FMNC_Test_Sequence;

/** The FMNC_Transmit_BufferBlock captures the behavior for the data that will likely
 * be sent that constitutes a single block for an overall test sequence.
 */
class FMNC_Transmit_BufferBlock
{
	public:
		FMNC_Transmit_BufferBlock ();

		/** Set the pointer for the current data block.  Note that it is the responsibility
		 * of the caller to have checked and handled any memory allocations if there is
		 * data that is allocated previously in this buffer block
		 * @param pData The new internal buffer
		 */
		void setData (char * pData);

		/** Retrieve a pointer to the data associated with this particular block
		 * @returns Pointer to the core data block (zero indexed)
		 */
		char * getData ();

		/** Set the length associated with this particular data block
		 *
		 */
		void setLength (uint32_t lLength);

		/** Retrieve the length of this data block
		 * @returns Number of bytes in the data block
		 */
		uint32_t getLength ();

		/** Set the status flag with regards to ownership.  If true, the data
		 * block is owned by whomever made this data block.  In other words, the
		 * block and its respective allocations should be freed when one is done
		 * using this block.  If it is false, it means that this is shared data that
		 * should not be freed.
		 * @param bOwnData The new state of ownership for this buffer block
		 */
		void setFlag_Ownership (bool bOwnData);

		/** Get the status flag with regards to ownership.  If true, the data
		 * block is owned by whomever made this data block.  In other words, the
		 * block and its respective allocations should be freed when one is done
		 * using this block.  If it is false, it means that this is shared data that
		 * should not be freed.
		 * @returns The flag for ownership of this block
		 */
		bool getFlag_Ownership ();

		static FMNC_Transmit_BufferBlock createSimple (int nMode);

		/** Allocate a new data block of the requested size
		 * @param lLength The new block size (in bytes)
		 * @returns True if successful, false otherwise
		 */
		bool allocateBlock (uint32_t lLength);

		void setPosition (uint32_t lPosition);
		uint32_t getPosition ();

		void populateSimple (int nMode);

	private:
		// What is the length of data?
		uint32_t	m_lLength;

		// Pointer to the data
		char *		m_pData;

		// Does this buffer own the data and should discard it when it is done?
		bool		m_bOwnData;

		// Overall position (start of block) for the overall buffer
		uint32_t	m_lPosition;
};

/** The FMNC_Transmit_Buffer represents a sequence of buffer blocks that can be
 * accessed for transmission to a particular client.  Various fill modes and garbage
 * modes may be specified without using actual allocated data.
 */
class FMNC_Transmit_Buffer
{
	public:
		FMNC_Transmit_Buffer ();
		~FMNC_Transmit_Buffer ();


		/** Copy data from this sequence of buffer blocks into the destination specified
		 * @param pDestination Where the data goes
		 * @param lStart The starting byte from which to access
		 * @param lLength The number of bytes to copy over
		 * @return The number of bytes copied
		 */
		uint32_t	copyData (char * pDestination, uint32_t lStart, uint32_t lLength);

		bool getFlag_FillGarbage ();
		void setFlag_FillGarbage (bool bFlag);

		int getFillMode ();
		void setFillMode (int nMode);

		/** Add a specific buffer block to the mix by appending it to the
		 * end of the list of buffer blocks that we have
		 */
		bool	addBufferBlock (FMNC_Transmit_BufferBlock * pBlock);

		/** Return the number of bytes in the buffer across all buffer blocks
		 * @returns Retrieve the total length
		 */
		uint32_t	getTotalLength ();

		/** Retrieve the number of buffer block objects present
		 * @returns The count in terms of objects
		 */
		int			getCount ();

		void updatePositions ();

		int	 findBufferWithPosition (uint32_t lPosition);

	private:
		vector<FMNC_Transmit_BufferBlock *> 	m_Data;

		bool	m_bFillGarbage;
		int		m_nFillMode;
};




class FMNC_Transmission_TCP
{
	public:
		FMNC_Transmission_TCP ();

		void setParent (FMNC_Test_Sequence * pParent);
		FMNC_Test_Sequence * getParent ();

		// Where does this particular transmssion start in terms of the buffer?

		void	setStart (uint32_t lStart);
		uint32_t	getStart ();

		// How much should be sent on this particular transmission in the payload?

		void 	setLength (uint32_t lLength);
		uint32_t	getLength();

		void	setSeqNum (uint32_t lSeqNum);
		uint32_t	getSeqNum();

		// Given a base sequence number (start of the buffer, start=0), recompute the
		// sequence number for this particular transmission as appropriates
		void	adjustSeqNum (uint32_t lBase);

		void	incrementTime   (timeval * pAdjTime);


		void	setOutTime (uint32_t lSec, uint32_t lUS);
		timeval * getOutTime ();

	private:
		FMNC_Test_Sequence * 	m_pParent;

		// What is the start byte of the buffer to send?
		uint32_t	m_lStart;

		// What is the length of this block to send?
		uint32_t	m_lLength;

		// What is the starting sequence number that I am sending for this transmission?
		uint32_t	m_lSeqNum;

		// When should this start on the outbound queue?
		timeval		m_timeOut;
};

class FMNC_Test_Sequence
{
	public:
		FMNC_Test_Sequence ();

		void	addTransmit (FMNC_Transmission_TCP * pTransmit);

		FMNC_Connection_TCP_Slice *	getConnection ();
		void setConnection (FMNC_Connection_TCP_Slice * pConnection);

		void populatePredefined (int nPredef);

		void adjustSeqNumber (uint32_t lSeqNum);

		void adjustDispatchTime (timeval * pAdjTime);

		// Shape the output queue and set the time from right now subject to the initial delay
		//  request
		void shapeOutput (uint32_t lMicroDelay, uint32_t lInitialDelay);

		/** Given a packet, pop the next item from the buffer to populate the data payload of the
		 * packet. Fill in the packet length and sequence number and packet payload.
		 */
		bool populatePacketWithNextSend (Packet * pPacket);

		/** Retrieve the next item that needs to be sent
		 */
		FMNC_Transmission_TCP * getNextSend ();

		bool	isDone ();

		void setNextToSend (int nNext);
		void incrementNextToSend ();
		int getNextToSend ();

	private:
		vector<FMNC_Transmission_TCP *>	m_Transmissions;
		FMNC_Transmit_Buffer			m_TransmitBuffer;

		// Which connection am I associated with?
		FMNC_Connection_TCP_Slice *		m_pConnection;

		int		m_nNextToSend;
};



#endif /* FMNC_TEST_SEQUENCE_H_ */
