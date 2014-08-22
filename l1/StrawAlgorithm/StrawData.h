/*
 * StrawData.h
 *
 *  Created on: Jul 29, 2014
 *      Author: Thomas Hancock (Thomas.hancock@cern.ch)
 */
 
#pragma once
#ifndef STRAWDATA_H_
#define STRAWDATA_H_

#include <sys/types.h>
#include <cstdint>
#include <iostream>
#include <cstring> // Allows the use of memcpy
#include <bitset> // Allows the bitset type to be used
#include <cmath> // Allows the use of the pow function

#include "StrawDetectorParameters.h"

namespace na62 {

struct STRAW_DATA_HDR {
	uint16_t packetLength;
	uint8_t triggerTypeWord;
	uint8_t chamber:2;
	uint8_t view:2;
	uint8_t halfView:2;
	uint8_t reserved:2;
} __attribute__ ((__packed__));

/* WARNING: When reading in the file, the endianness of each byte has been reversed */
class StrawData {
public:
	StrawData(char* pointerToData);
	void loadHit(int hitNum);
	inline int getSrbID(uint8_t chamber, uint8_t view, uint8_t halfView);
	double getStrawDistance();
// getter functions
	inline uint8_t getL0TriggerDecision() {return m_strawDataHdr.triggerTypeWord;}
	inline uint8_t getChamber() {return m_strawDataHdr.chamber;}
	inline uint8_t getView() {return m_strawDataHdr.view;}
	inline uint8_t getHalfView() {return m_strawDataHdr.halfView;}
	inline uint16_t getPacketLength() {return m_strawDataHdr.packetLength;}
	inline uint16_t getCoarseTime() {return m_coarseTime;}
	inline uint8_t getError() {return m_error;}
	inline uint8_t getStrawID() {return m_strawID;}
	inline uint8_t getPlaneID() {return m_planeID;}
	inline uint8_t getEdgeType() {return m_edgeType;}
	inline double getFineTime () {return m_fineTime;}
	inline uint8_t getNEdgesSlot (int n) { 
		if (n < 16) {
			return m_nEdgesArray[n];
		} else {
			return 0;
		}
	}
	inline int getNumberOfHits() {return m_numberOfHits;}
// Debugging Functions
	void printHeader();
	void printNEdgesSlots();
	void printHit();
	
private:
	void readL0TriggerDecision();
	void readChamber();
	void readView();
	void readHalfView();
	void readPacketLength();
	void readCoarseTime();
	void readNEdgesSlots();
	void readDataWord(int hitNum);
	void initialiseHitVariables();
	double decodeFineTime(int tempFineTime);
	void separateStrawAndPlaneIDs();
// Header Variables
//	uint8_t m_strawDataHdr.triggerTypeWord, m_chamber, m_view, m_halfView;
//	uint16_t m_packetLength = 0;
	uint32_t m_coarseTime;
	STRAW_DATA_HDR m_strawDataHdr;
// Data word variables
	uint8_t m_error;
	uint8_t m_strawID;
	uint8_t m_planeID;
	uint8_t m_edgeType;
	double m_fineTime;
// N edges slots
	uint8_t m_nEdgesArray[16];
// Other variables
	char* dataPointer; // Stores the location of the data in memory
	int m_numberOfHits;
};

} /* namespace na62 */

#endif /* STRAWDATA_H_ */