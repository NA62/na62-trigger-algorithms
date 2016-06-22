/*
 * L1InfoToStorage.cpp
 *
 *  Created on: 2 Nov 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L1InfoToStorage.h"

#include <iostream>
#include <vector>
#include <options/Logging.h>
#include <string.h>

#include <sys/time.h>

L1InfoToStorage* L1InfoToStorage::theInstance = nullptr;

L1InfoToStorage::L1InfoToStorage() {
//	LOG_INFO("********************In L1 InfoToStorage");
	chodTime = 0.;
	nKTAGSectors_l0tp = 0;
	nKTAGSectors_chod = 0;
	nCHODHits = 0;
	nLAVHits = 0;
	nMUV3Tiles = 0;
	l1CHODProcessed_ = false;
	l1CHODEmptyPacket_ = false;
	l1CHODBadData_ = false;
	l1KTAGProcessed_ = false;
	l1KTAGEmptyPacket_ = false;
	l1KTAGBadData_ = false;
	l1LAVProcessed_ = false;
	l1LAVEmptyPacket_ = false;
	l1LAVBadData_ = false;
	l1MUV3Processed_ = false;
	l1MUV3EmptyPacket_ = false;
	l1MUV3BadData_ = false;
}

L1InfoToStorage::~L1InfoToStorage() {
}

L1InfoToStorage* L1InfoToStorage::GetInstance() {

	if (theInstance == nullptr) {
		theInstance = new L1InfoToStorage();
	}
	return theInstance;

}

double L1InfoToStorage::getCHODAverageTime() {
	return chodTime;
}

void L1InfoToStorage::setCHODAverageTime(double time) {
	chodTime = time;
}

uint L1InfoToStorage::getL1CHODNHits() {
	return nCHODHits;
}

void L1InfoToStorage::setL1CHODNHits(uint nHits) {
	nCHODHits = nHits;
}

uint L1InfoToStorage::getL1KTAGNSectors_l0tp() {
	return nKTAGSectors_l0tp;
}

void L1InfoToStorage::setL1KTAGNSectors_l0tp(uint nSectors) {
	nKTAGSectors_l0tp = nSectors;
}

uint L1InfoToStorage::getL1KTAGNSectors_chod() {
	return nKTAGSectors_chod;
}

void L1InfoToStorage::setL1KTAGNSectors_chod(uint nSectors) {
	nKTAGSectors_chod = nSectors;
}

uint L1InfoToStorage::getL1LAVNHits() {
	return nLAVHits;
}
void L1InfoToStorage::setL1LAVNHits(uint nHits) {
	nLAVHits = nHits;
}

uint L1InfoToStorage::getL1MUV3NTiles() {
	return nMUV3Tiles;
}

void L1InfoToStorage::setL1MUV3NTiles(uint nTiles) {
	nMUV3Tiles = nTiles;
}

