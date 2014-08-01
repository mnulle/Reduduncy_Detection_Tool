/*
 * fmnc_test_sequence.cc
 *
 *  Created on: Jun 4, 2014
 *      Author: striegel
 */

#include <iostream>
using namespace std;

#include "../util/ip-utils.h"

#include "fmnc_test_sequence.h"

///////////////////////////////////////////////////

FMNC_Transmit_BufferBlock::FMNC_Transmit_BufferBlock ()
{
	m_lLength = 0;
	m_pData = NULL;
	m_bOwnData = true;
	m_lPosition = 0;
}

///////////////////////////////////////////////////

void FMNC_Transmit_BufferBlock::setData (char * pData)
{
	m_pData = pData;
}

///////////////////////////////////////////////////

void FMNC_Transmit_BufferBlock::populateSimple (int nMode)
{
	// Giant comment for the purposes of rendering

	// Assume we know the size

	// TODO: Make this faster (maybe)

	m_pData[0] = '<';
	m_pData[1] = '!';
	m_pData[2] = '-';
	m_pData[3] = '-';
	m_pData[4] = ' ';

	int		j;

	for(j=5; j<m_lLength-5; j++)
	{
		m_pData[j] = 'A' + (j%26);
	}

	m_pData[m_lLength-4] = ' ';
	m_pData[m_lLength-3] = '-';
	m_pData[m_lLength-2] = '-';
	m_pData[m_lLength-1] = '>';
}

///////////////////////////////////////////////////

char * FMNC_Transmit_BufferBlock::getData ()
{
	return m_pData;
}

///////////////////////////////////////////////////

void FMNC_Transmit_BufferBlock::setLength (uint32_t lLength)
{
	m_lLength = lLength;
}

///////////////////////////////////////////////////

uint32_t FMNC_Transmit_BufferBlock::getLength ()
{
	return m_lLength;
}

///////////////////////////////////////////////////

void FMNC_Transmit_BufferBlock::setFlag_Ownership (bool bOwnData)
{
	m_bOwnData = bOwnData;
}

///////////////////////////////////////////////////

bool FMNC_Transmit_BufferBlock::getFlag_Ownership ()
{
	return m_bOwnData;
}

///////////////////////////////////////////////////

bool FMNC_Transmit_BufferBlock::allocateBlock (uint32_t lLength)
{
	m_pData = new char[lLength];

	if(m_pData == NULL)
	{
		m_lLength = 0;
		return false;
	}
	else
	{
		m_lLength = lLength;
		return true;
	}
}

///////////////////////////////////////////////////

void FMNC_Transmit_BufferBlock::setPosition (uint32_t lPosition)
{
	m_lPosition = lPosition;
}

///////////////////////////////////////////////////

uint32_t FMNC_Transmit_BufferBlock::getPosition ()
{
	return m_lPosition;
}



FMNC_Transmit_Buffer::FMNC_Transmit_Buffer ()
{
	m_bFillGarbage = true;
	m_nFillMode = 0;
}

FMNC_Transmit_Buffer::~FMNC_Transmit_Buffer ()
{
	int		j;

	for(j=0; j<m_Data.size(); j++)
	{
		if(m_Data[j]->getFlag_Ownership() && m_Data[j]->getData() != NULL)
		{
			delete m_Data[j]->getData();
			m_Data[j]->setData(NULL);
		}
	}

}

void FMNC_Transmit_Buffer::updatePositions ()
{
	int		j;
	uint32_t	lTotal;

	lTotal = 0;

	for(j=0; j<m_Data.size(); j++)
	{
		m_Data[j]->setPosition(lTotal);
		lTotal += m_Data[j]->getLength();
	}
}

uint32_t FMNC_Transmit_Buffer::copyData (char * pDestination, uint32_t lStart, uint32_t lLength)
{
	// TODO: Consider removing this later
	updatePositions();

	// Do this recursively if appropriate

	int 	nPos;

	nPos = findBufferWithPosition(lStart);

	if(nPos < 0)
	{
		// Error, we copied nothing
		cerr << " * Error: Could not copy starting at byte " << lStart << " of length " << lLength << endl;
		return 0;
	}

	// OK, we have a good block.  Now does this block contain everything we need or do we need to
	//  span one or more blocks (will do that recursively)

	uint32_t	lOffset;

	// How far into this block are we currently?
	lOffset = lStart - m_Data[nPos]->getPosition();

	if(m_Data[nPos]->getLength() - lOffset >= lLength)
	{
		// We are done with this one here
		memcpy(pDestination, m_Data[nPos]->getData()+lOffset, lLength);
		return lLength;
	}
	else
	{
		// Nope, more than just this block
		uint32_t	lPartialCover;

		lPartialCover = m_Data[nPos]->getLength()-lOffset;

		// Step 1 - Copy what we do have in this block
		memcpy(pDestination, m_Data[nPos]->getData()+lOffset, lPartialCover);

		return lPartialCover + copyData(pDestination+lPartialCover, lStart+lPartialCover, lLength-lPartialCover);
	}

	return 0;
}

int	 FMNC_Transmit_Buffer::findBufferWithPosition (uint32_t lPosition)
{
	int		j;

	uint32_t	lRemaining;

	lRemaining = lPosition;

	for(j=0; j<m_Data.size(); j++)
	{
		if(lRemaining < m_Data[j]->getLength())
		{
			return j;
		}

		lRemaining -= m_Data[j]->getLength();
	}

	// Nope, did not find it at all
	return -1;
}

bool FMNC_Transmit_Buffer::getFlag_FillGarbage ()
{
	return m_bFillGarbage;
}

void FMNC_Transmit_Buffer::setFlag_FillGarbage (bool bFlag)
{
	m_bFillGarbage = bFlag;
}

int FMNC_Transmit_Buffer::getFillMode ()
{
	return m_nFillMode;
}

void FMNC_Transmit_Buffer::setFillMode (int nMode)
{
	m_nFillMode = nMode;
}

bool FMNC_Transmit_Buffer::addBufferBlock (FMNC_Transmit_BufferBlock * pBlock)
{
	if(pBlock != NULL)
	{
		m_Data.push_back(pBlock);
		return true;
	}
	else
	{
		cerr << " Error: Did not add a null buffer block to the transmission buffer" << endl;
		cerr << "   fmnc_test_sequence.cc (addBufferBlock)" << endl;
		return false;
	}
}

uint32_t FMNC_Transmit_Buffer::getTotalLength ()
{
	int		j;
	uint32_t	lTotal;

	lTotal = 0;

	for(j=0; j<m_Data.size(); j++)
	{
		lTotal += m_Data[j]->getLength();
	}

	return lTotal;
}

int	FMNC_Transmit_Buffer::getCount ()
{
	return m_Data.size();
}

///////////////////////////////////////////////////////////////////////////////////

FMNC_Transmission_TCP::FMNC_Transmission_TCP()
{
	m_pParent = NULL;
	m_lStart = 0;
	m_lLength = 0;
	m_lSeqNum = 0;
}

///////////////////////////////////////////////////////////////////////////////////

void FMNC_Transmission_TCP::setParent (FMNC_Test_Sequence * pParent)
{
	m_pParent = pParent;
}

///////////////////////////////////////////////////////////////////////////////////

FMNC_Test_Sequence * FMNC_Transmission_TCP::getParent ()
{
	return m_pParent;
}

void FMNC_Transmission_TCP::setStart (uint32_t lStart)
{
	m_lStart = lStart;
}

uint32_t FMNC_Transmission_TCP::getStart ()
{
	return m_lStart;
}

void 	FMNC_Transmission_TCP::setLength (uint32_t lLength)
{
	m_lLength = lLength;
}

uint32_t FMNC_Transmission_TCP::getLength()
{
	return m_lLength;
}

void FMNC_Transmission_TCP::setSeqNum (uint32_t lSeqNum)
{
	m_lSeqNum = lSeqNum;
}

uint32_t FMNC_Transmission_TCP::getSeqNum()
{
	return m_lSeqNum;
}

void FMNC_Transmission_TCP::adjustSeqNum (uint32_t lBase)
{
	m_lSeqNum += lBase;
}

void FMNC_Transmission_TCP::incrementTime (timeval * pAdjTime)
{
	timeval 	adjTime;

	// TODO: Fix this better in the future to actually add two.  We are taking advantage of the
	//       fact that the base time in this is unlikely to be anything but microseconds
	adjTime = *pAdjTime;

	adjustTime(&adjTime, m_timeOut.tv_usec);

	m_timeOut = adjTime;
}

void FMNC_Transmission_TCP::setOutTime (uint32_t lSec, uint32_t lUS)
{
	m_timeOut.tv_sec = lSec;
	m_timeOut.tv_usec = lUS;
}

timeval * FMNC_Transmission_TCP::getOutTime ()
{
	return &m_timeOut;
}




///////////////////////////////////////////////////////////////////////////////////

FMNC_Test_Sequence::FMNC_Test_Sequence ()
{
	m_pConnection = NULL;
	m_nNextToSend = 0;
}

void FMNC_Test_Sequence::setNextToSend (int nNext)
{
	m_nNextToSend = nNext;
}

void FMNC_Test_Sequence::incrementNextToSend ()
{
	m_nNextToSend++;
}

int FMNC_Test_Sequence::getNextToSend ()
{
	return m_nNextToSend;
}

bool FMNC_Test_Sequence::isDone ()
{
	if(m_nNextToSend >= m_Transmissions.size())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool FMNC_Test_Sequence::populatePacketWithNextSend (Packet * pPacket)
{
	cout << "Populating packet " << m_nNextToSend << " in the test sequence" << endl;

	FMNC_Transmission_TCP *	pTransmit;

	if(m_nNextToSend >= m_Transmissions.size())
	{
		cout << "* Error: Asked to send the block but do not have anything to send" << endl;
		return false;
	}

	pTransmit = m_Transmissions[m_nNextToSend];

	cout << "   Slice of " << pTransmit->getLength() << " bytes with a sequence number of " << pTransmit->getSeqNum() << endl;

	// TODO : Fix this to use better lengths rather than hard coding
	pPacket->setLength(14+20+20+pTransmit->getLength());

	// TODO: Fix the absolute length (No L2 for IP length FYI)
	setPacketLength(pPacket->getData(), pTransmit->getLength() + 20 + 20);

	// Copy in our data to the payload
	char * pPayload;

	// TODO : Fix this to use better lengths rather than hard coding
	pPayload = pPacket->getData() + 14 + 20 + 20;

	// Where does this particular packet need to get its data from?
	m_TransmitBuffer.copyData(pPayload, pTransmit->getStart(), pTransmit->getLength());

	// Set the sequence number
	writeTCP_SeqNumber(pTransmit->getSeqNum(), pPacket->getData(), pPacket->getLength());

	// Clear any flags (we don't ACK while slicing)
	clearTCP_Flags(pPacket->getData());

	updateIPv4_Checksum(pPacket->getData(), pPacket->getLength());
	updateTCP_Checksum(pPacket->getData(), pPacket->getLength());
	updateIPv4_Checksum(pPacket->getData(), pPacket->getLength());

	// Move to the next item to send
	incrementNextToSend();

	return true;
}


void FMNC_Test_Sequence::addTransmit (FMNC_Transmission_TCP * pTransmit)
{
	m_Transmissions.push_back(pTransmit);
}

FMNC_Connection_TCP_Slice *	FMNC_Test_Sequence::getConnection ()
{
	return m_pConnection;
}


void FMNC_Test_Sequence::setConnection (FMNC_Connection_TCP_Slice * pConnection)
{
	m_pConnection = pConnection;
}

void FMNC_Test_Sequence::adjustSeqNumber (uint32_t lSeqNum)
{
	int		j;

	for(j=0; j<m_Transmissions.size(); j++)
	{
		m_Transmissions[j]->adjustSeqNum (lSeqNum);
	}
}


void FMNC_Test_Sequence::adjustDispatchTime (timeval * pAdjTime)
{
	int		j;

	for(j=0; j<m_Transmissions.size(); j++)
	{
		m_Transmissions[j]->incrementTime(pAdjTime);
	}
}


void FMNC_Test_Sequence::populatePredefined (int nPredef)
{
	FMNC_Transmission_TCP *	pPredef;

	pPredef = new FMNC_Transmission_TCP();

	pPredef->setParent(this);

	// Add a simple transmission sequence of 200 bytes

	// Create the data first off
	FMNC_Transmit_BufferBlock	*	pBlock;

	pBlock = new FMNC_Transmit_BufferBlock();
	pBlock->setData( new char[100]);
	pBlock->setLength(100);
	pBlock->populateSimple(0);

	m_TransmitBuffer.addBufferBlock(pBlock);

	pBlock = new FMNC_Transmit_BufferBlock();
	pBlock->setData( new char[200]);
	pBlock->setLength(200);
	pBlock->populateSimple(0);

	m_TransmitBuffer.addBufferBlock(pBlock);


	FMNC_Transmission_TCP	*	pSlice;
	int		j;

	// TODO: Add timing for the respective packets
	//		1 ms spacing starting from now

	for(j=1; j<6; j++)
	{
		pSlice = new FMNC_Transmission_TCP();
		pSlice->setStart(j*50);
		pSlice->setLength(50);
		// 1000 microseconds
		pSlice->setOutTime(0, 1000*(j-1));
		m_Transmissions.push_back(pSlice);
	}

	pSlice = new FMNC_Transmission_TCP();
	pSlice->setStart(0);
	pSlice->setLength(50);
	pSlice->setOutTime(0, 1000*5);
	m_Transmissions.push_back(pSlice);
}


