/*
 * fmnc_measurement_packet.cc
 *
 *  Created on: Jun 9, 2014
 *      Author: Aaron
 */


#include "fmnc_measurement_packet.h"

#include <iostream>
using namespace std;

#include "../util/ip_utils.h"

FMNC_Measurement_Packet::FMNC_Measurement_Packet()
{
	m_sMetaData = "";
}

void FMNC_Measurement_Packet::setMetaData (string sData)
{
	m_sMetaData = sData;
}

void FMNC_Measurement_Packet::appendMetaData (string sAppend)
{
	m_sMetaData += sAppend;
}

string FMNC_Measurement_Packet::getMetaData ()
{
	return m_sMetaData;
}

void FMNC_Measurement_Packet::setime (struct timeval * pTime)
{
	m_Time = *pTime;
}

struct timeval * FMNC_Measurement_Packet::getTime ()
{
	return &m_Time;
}

bool FMNC_Measurement_Packet::instrumentPacket (Packet * pPacket)
{
	return false;
}

bool FMNC_Measurement_Packet::instrumentPacket_Base ()
{
	m_Time = *(pPacket->getArrivalTime());
}

string 	FMNC_Measurement_Packet::getXML ()
{
	string 	sXML;

	sXML = "<Packet ";

	sXML += getAttribute_Time();

	return sXML;
}

string FMNC_Measurement_Packet::getXML_Attributes_Base ()
{
	String 	sXML;

	sXML = getAttribute_Time();
	sXML += getAttribute_MetaData();

	return sXML;
}

string	FMNC_Measurement_Packet::getAttribute_MetaData ()
{
	String	sXML;

	sXML = "";

	if(m_sMetaData.length() > 0)
	{
		sXML += "Meta=\"" + m_sMetaData + "\"";
	}

	return sXML;
}

string	FMNC_Measurement_Packet::getAttribute_Time ()
{
	string 	sXML;
	char 	szTemp[50];

	sXML = "Time=\"";

	sprintf(szTemp, "%ld.%ld\"", m_Time.tv_sec, m_Time.tv_usec);

	sXML += szTemp;

	return sXML;
}

string 	FMNC_Measurement_Packet::getAttribute_Time (timeval * pBaseTime)
{
	uint32_t		lTimeDiff;
	char 	szTemp[50];
	string 	sXML;

	lTimeDiff = calcTimeDiff (pBaseTime, &m_Time);

	uint32_t		lSec;
	uint32_t		lMicroSec;

	lMicroSec = lTimeDiff % 1000000;
	lSec = lTimeDiff / 1000000;

	sXML = "Time=\"";
	sprintf(szTemp, "%ld.%ld\"", lSec, lMicroSec);
	sXML += szTemp;
	return sXML;
}

string 	FMNC_Measurement_Packet::getAttribute_Time (FMNC_Measurement_Packet * pBasePacket)
{
	return getAttribute_Time(pBasePacket->getTime());
}

string 	FMNC_Measurement_Packet::getTime_AsString ()
{
	string 	sTime;
	char 	szTemp[50];

	sTime = "";

	sprintf(szTemp, "%ld.%ld\"", m_Time.tv_sec, m_Time.tv_usec);

	sTime += szTemp;

	return sTime;
}


FMNC_List_Measurement_Packets::FMNC_List_Measurement_Packets ()
{

}

FMNC_List_Measurement_Packets::~FMNC_List_Measurement_Packets ()
{
	int		j;

	for(j=0; j<m_Packets.size(); j++)
	{
		delete m_Packets[j];
	}

	m_Packets.clear();
}

string FMNC_List_Measurement_Packets::getXML(string sTag)
{
	int		j;
	string	sXML;

	sXML = "<" + sTag + " Count=\"" + m_Packets.size() + "\">";

	for(j=0; j<m_Packets.size(); j++)
	{
		sXML += m_Packets[j]->getXML();
	}

	sXML += "</" + sTag + ">";

	return sXML;
}

void  FMNC_List_Measurement_Packets::addPacket (FMNC_Measurement_Packet * pPacket)
{
	if(pPacket != NULL)
	{
		m_pPackets.push_back(pPacket);
	}
}


FMNC_Measurement_Packet_IPv4::FMNC_Measurement_Packet_IPv4 () : FMNC_Measurement_Packet ()
{
	m_byTTL = 0;
	m_byTOS = 0;
	m_nLength = 0;
	m_nID = 0;
}

void	FMNC_Measurement_Packet_IPv4::setTTL (uint8_t byTTL)
{
	m_byTTL = byTTL;
}

uint8_t	FMNC_Measurement_Packet_IPv4::getTTL ()
{
	return m_byTTL;
}

void	FMNC_Measurement_Packet_IPv4::setTOS (uint8_t byTOS)
{
	m_byTOS = byTOS;
}


uint8_t	FMNC_Measurement_Packet_IPv4::getTOS ()
{
	return m_byTOS;
}

void	FMNC_Measurement_Packet_IPv4::setLength (uint16_t nLength)
{
	m_nLength = nLength;
}

uint16_t	FMNC_Measurement_Packet_IPv4::getLength ()
{
	return m_nLength;
}


void FMNC_Measurement_Packet_IPv4::setID (uint16_t nID)
{
	m_nID = nID;
}

uint16_t FMNC_Measurement_Packet_IPv4::getID ()
{
	return m_nID;
}

bool instrumentPacket (Packet * pPacket)
{
	return instrumentPacket_IP(pPacket);
}

string 	FMNC_Measurement_Packet_IPv4::getXML ()
{
	String 	sXML;

	sXML += "<PacketIPv4 ";

	sXML += getXML_Attributes_Base();
	sXML += getXML_Attributes_IPv4();

	sXML += " />";

	return sXML;
}

string FMNC_Measurement_Packet_IPv4::getXML_Attributes_IPv4 ()
{
	String	sXML;

	sXML = "TTL=\"" + m_byTTL + "\" TOS=\"" + m_byTOS + "\" Length=\"" + m_nLength + "\" ID=\"" + m_nID + "\"";

	return sXML;
}


bool FMNC_Measurement_Packet_IPv4::instrumentPacket_IPv4 (Packet * pPacket)
{
	instrumentPacket_Base(pPacket);

	m_nLength = getPacketLength(pPacket->getData(), pPacket->getLength());

	m_byTTL = pPacket->getData() + L2_OFFSET + OFFSET_IPV4_TTL;
	m_byTOS = pPacket->getData() + L2_OFFSET + OFFSET_IPV4_TOS;

	m_nID = getIPv4_ID(pPacket->getData(), pPacket->getLength());

	return true;
}

FMNC_Measurement_Packet_SYN::FMNC_Measurement_Packet_SYN() : FMNC_Measurement_Packet_IPv4 ()
{
	memset(m_byOptions, 0, 100);
	m_byOptionLength = 0;
	m_lClientSeq = 0;
}

bool FMNC_Measurement_Packet_SYN::instrumentPacket (Packet * pPacket)
{
	instrumentPacket_IPv4(pPacket);

	m_byOptionLength = (uint8_t) getTCPOptions_Size (pPacket->getData());

	if(m_byOptionlength <= 100)
	{
		memcpy(m_byOptions, pPacket->getData() + OFFSET_IPV4_DATA, m_byOptionLength);
	}
	else
	{
		cerr << "* Warning: TCP Option length is viewed to be more than 100 bytes, capping at 100" << endl;
		memcpy(m_byOptions, pPacket->getData() + OFFSET_IPV4_DATA, 100);
	}

	m_lClientSeq = getTCP_SeqNumber(pPacket->getData(), pPacket->getLength());

	return true;
}

void	FMNC_Measurement_Packet_SYN::setOptionLength (uint8_t byLength)
{
	m_byOptionLength = byLength;
}

uint8_t	FMNC_Measurement_Packet_SYN::getOptionLength ()
{
	return m_byOptionLength;
}

string 	FMNC_Measurement_Packet_SYN::getXML ()
{
	String sXML;

	sXML = "<PktSYN ";

	sXML += getXML_Attributes_Base();
	sXML += getXML_Attributes_IPv4();

	if(m_byOptionLength > 0)
	{
		char	szTemp[5];

		sXML += " OpLen=\"" + m_byOptionLength + "\" Options=\"";

		for(int j=0; j<m_byOptionLength; j++)
		{
			sprintf(szTemp, "%02X", m_byOptions[j]);
		}

		sXML += "\"";
	}

	sXML += "ClientSN=\"" + m_lClientSeq + "\"";

	sXML += " />";

	return sXML;
}

void	FMNC_Measurement_Packet_SYN::setClientSeqNum (uint32_t lSeqNum)
{
	m_lClientSeq = lSeqNum;
}

uint32_t FMNC_Measurement_Packet_SYN::getClientSeqNum ()
{
	return m_lClientSeq;
}


FMNC_Measurement_Packet_TCP::FMNC_Measurement_Packet_TCP () : FMNC_Measurement_Packet_IPv4 ()
{
	m_bFlagAck = false;
	m_lSeqNum = 0;
	m_lAckNum = 0;
}

string 	FMNC_Measurement_Packet_TCP::getXML ()
{
	String sXML;

	sXML = "<PktTCP ";

	sXML += getXML_Attributes_Base();
	sXML += getXML_Attributes_IPv4();

	sXML += "SN=\"" + m_lSeqNum + "\"";

	if(m_bFlagAck)
	{
		sXML += "AN=\"" + m_lAckNum + "\"";
	}

	sXML += " />";

	return sXML;
}

void	FMNC_Measurement_Packet_TCP::setSeqNum (uint32_t lSeqNum)
{
	m_lSeqNum = lSeqNum;
}

uint32_t FMNC_Measurement_Packet_TCP::getSeqNum ()
{
	return m_lSeqNum;
}

void	FMNC_Measurement_Packet_TCP::setAckNum (uint32_t lAckNum)
{
	m_lAckNum = lAckNum;
}

uint32_t FMNC_Measurement_Packet_TCP::getAckNum ()
{
	return m_lAckNum;
}

bool FMNC_Measurement_Packet_TCP::instrumentPacket (Packet * pPacket)
{
	instrumentPacket_IPv4(pPacket);

	m_lSeqNum = getTCP_SeqNumber(pPacket->getData(), pPacket->getLength());
	m_lAckNum = getTCP_AckNumber(pPacket->getData(), pPacket->getLength());

	if(getTCPFlag_ACK(pPacket->getData(), pPacket->getLength()))
	{
		m_bFlagAck = true;
	}
	else
	{
		m_bFlagAck = false;
	}

	return true;
}


