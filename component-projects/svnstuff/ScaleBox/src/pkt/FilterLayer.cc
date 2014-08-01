
#include <iostream>
#include <string>
using namespace std;

#include "../xml/NodeDOM.h"
#include "../xml/NodeElem.h"
#include "FilterLayer.h"
#include "Packet.h"

FilterLayer::FilterLayer() {
	
}

FilterLayer::~FilterLayer () {
	
}

FilterL2Ethernet::FilterL2Ethernet () {
	m_bFilterSrcMAC = false;
	m_bFilterDstMAC = false;
	m_bFilterTypeLen = false;
}		

FilterL2Ethernet::~FilterL2Ethernet () {
	
}

char	FilterL2Ethernet::matchFilter (Packet * pPacket) {
	//cout << "      -> matchFilter (FilterL2Ethernet)" << endl;
	
	// We operate from a pass/no pass. If it does not violate any of the
	//  requested matches, it passes
	
	if(m_bFilterSrcMAC) {
		if(memcmp(m_bySrcMAC, pPacket->getData()+0, ETH_MAC_LENGTH) != 0) {
			return 0;
		}
	}
	
	if(m_bFilterDstMAC) {
		if(memcmp(m_byDstMAC, pPacket->getData()+ETH_MAC_LENGTH, ETH_MAC_LENGTH) != 0) {
			return 0;
		}		
	}
	
	if(m_bFilterTypeLen) {
		if(memcmp(m_byTypeLen, pPacket->getData()+ETH_MAC_LENGTH*2, ETH_MAC_TYPELEN) != 0) {
			return 0;
		}
	}
	
	return 1;	
}
		
void	FilterL2Ethernet::enableFilterSrcMAC (bool bEnable) {
	m_bFilterSrcMAC = bEnable;
}

void	FilterL2Ethernet::setFilterSrcMAC (char * byMAC) {
	memcpy(m_bySrcMAC, byMAC, ETH_MAC_LENGTH);
}

void	FilterL2Ethernet::enableFilterDstMAC (bool bEnable) {
	m_bFilterDstMAC = bEnable;	
}

void	FilterL2Ethernet::setFilterDstMAC (char * byMAC) {
	memcpy(m_byDstMAC, byMAC, ETH_MAC_LENGTH);	
}

void	FilterL2Ethernet::enableFilterTypeLen (bool bEnable) {
	m_bFilterTypeLen = bEnable;	
}

void	FilterL2Ethernet::setFilterTypeLen (char * byTypeLen) {
	memcpy(m_byTypeLen, byTypeLen, ETH_MAC_TYPELEN);
}

bool	FilterL2Ethernet::populateFromDOM (NodeDOM * pNode) {
	int			j;
	
	// Iterate through the elements
	for(j=0; j<pNode->getNumElements(); j++) {
		string		sElem;	
		
		sElem = pNode->getElement(j)->getName();
		
		if(sElem == "TypeLen") {
			unsigned short		nTypeLen;
			
			sscanf(pNode->getElement(j)->getValue().c_str(), "0x%X", &nTypeLen);
			
			printf("  TypeLen is 0x%X\n", nTypeLen);
			
			m_bFilterTypeLen = true;			
		} else {
			cerr << "Warning: Unknown filter component for the Ethernet filter" << endl;
			cerr << "  Requested filter of " << sElem << " will be ignored" << endl;	
		}
	}
	
	if(m_bFilterTypeLen || m_bFilterDstMAC || m_bFilterDstMAC) { 
		return true;	
	} else {
		cerr << "Error: No actual filter fields specified for the Ethernet filter" << endl;
		return false;
	}
}

FilterLayer * createFromDOM (NodeDOM * pNode) {
	// Validate the object is not NULL
	if(pNode == NULL) {
		return NULL;
	}
	
	// Select the appropriate case
	if(pNode->getTag() == "Ethernet") {
		FilterL2Ethernet 	*	pEthernet;
		
		pEthernet = new FilterL2Ethernet();
		
		if(pEthernet->populateFromDOM(pNode)) {
			return pEthernet;
		} else {
			delete pEthernet;
			return NULL;
		}
	} else {
		cerr << "Warning: Unknown tag for layer filter of " << pNode->getTag() << endl;
		cerr << "   Ignoring the tag" << endl;	
		return NULL;
	}
	
	return NULL;	
}		
	
