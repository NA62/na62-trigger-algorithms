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

L1InfoToStorage::L1InfoToStorage() {
//	LOG_INFO("********************In L1 InfoToStorage");
	chodTime_ = 0.;
	newchodTime_ = 0.;
	refTimeL0TP_ = 0;

	l1CHODTrgWrd_ = 0;
	l1KTAGTrgWrd_ = 0;
	l1LAVTrgWrd_ = 0;
	l1IRCSACTrgWrd_ = 0;
	l1StrawTrgWrd_ = 0;
	l1MUV3TrgWrd_ = 0;
	l1NewCHODTrgWrd_ = 0;

	l1CHODProcessed_ = false;
	l1KTAGProcessed_ = false;
	l1LAVProcessed_ = false;
	l1IRCSACProcessed_ = false;
	l1StrawProcessed_ = false;
	l1MUV3Processed_ = false;
	l1NewCHODProcessed_ = false;

	l1CHODEmptyPacket_ = false;
	l1KTAGEmptyPacket_ = false;
	l1LAVEmptyPacket_ = false;
	l1IRCSACEmptyPacket_ = false;
	l1StrawEmptyPacket_ = false;
	l1MUV3EmptyPacket_ = false;
	l1NewCHODEmptyPacket_ = false;

	l1CHODBadData_ = false;
	l1KTAGBadData_ = false;
	l1LAVBadData_ = false;
	l1IRCSACBadData_ = false;
	l1StrawBadData_ = false;
	l1MUV3BadData_ = false;
	l1NewCHODBadData_ = false;

	l1StrawOverflow_ = false;

	nCHODHits_ = 0;
	nKTAGSectorsL0TP_ = 0;
	nKTAGSectorsCHOD_ = 0;
	nLAVHits_ = 0;
	nIRCSACHits_ = 0;
	nStrawTracks_ = 0;
	nMUV3Tiles_ = 0;
	nNewCHODHits_ = 0;

	for (uint iTrk = 0; iTrk != 5; iTrk++) {
		trackP_[iTrk] = 0.;
		trackVz_[iTrk] = 0.;
	}
}

L1InfoToStorage::~L1InfoToStorage() {
}

uint_fast8_t L1InfoToStorage::getL1CHODTrgWrd() {
	return l1CHODTrgWrd_;
}
void L1InfoToStorage::setL1CHODTrgWrd(uint_fast8_t trigger) {
	l1CHODTrgWrd_ = trigger;
}

uint_fast8_t L1InfoToStorage::getL1KTAGTrgWrd() {
	return l1KTAGTrgWrd_;
}
void L1InfoToStorage::setL1KTAGTrgWrd(uint_fast8_t trigger) {
	l1KTAGTrgWrd_ = trigger;
}

uint_fast8_t L1InfoToStorage::getL1LAVTrgWrd() {
	return l1LAVTrgWrd_;
}
void L1InfoToStorage::setL1LAVTrgWrd(uint_fast8_t trigger) {
	l1LAVTrgWrd_ = trigger;
}

uint_fast8_t L1InfoToStorage::getL1IRCSACTrgWrd() {
	return l1IRCSACTrgWrd_;
}
void L1InfoToStorage::setL1IRCSACTrgWrd(uint_fast8_t trigger) {
	l1IRCSACTrgWrd_ = trigger;
}

uint_fast8_t L1InfoToStorage::getL1StrawTrgWrd() {
	return l1StrawTrgWrd_;
}
void L1InfoToStorage::setL1StrawTrgWrd(uint_fast8_t trigger) {
	l1StrawTrgWrd_ = trigger;
}

uint_fast8_t L1InfoToStorage::getL1MUV3TrgWrd() {
	return l1MUV3TrgWrd_;
}
void L1InfoToStorage::setL1MUV3TrgWrd(uint_fast8_t trigger) {
	l1MUV3TrgWrd_ = trigger;
}

uint_fast8_t L1InfoToStorage::getL1NewCHODTrgWrd() {
	return l1NewCHODTrgWrd_;
}
void L1InfoToStorage::setL1NewCHODTrgWrd(uint_fast8_t trigger) {
	l1NewCHODTrgWrd_ = trigger;
}

uint_fast8_t L1InfoToStorage::getL1RefTimeL0TP() {
	return refTimeL0TP_;
}
void L1InfoToStorage::setL1RefTimeL0TP(uint_fast8_t time) {
	refTimeL0TP_ = time;
}

double L1InfoToStorage::getCHODAverageTime() {
	return chodTime_;
}
void L1InfoToStorage::setCHODAverageTime(double time) {
	chodTime_ = time;
}
uint L1InfoToStorage::getL1CHODNHits() {
	return nCHODHits_;
}
void L1InfoToStorage::setL1CHODNHits(uint nHits) {
	nCHODHits_ = nHits;
}

uint L1InfoToStorage::getL1KTAGNSectorsL0TP() {
	return nKTAGSectorsL0TP_;
}
void L1InfoToStorage::setL1KTAGNSectorsL0TP(uint nSectors) {
	nKTAGSectorsL0TP_ = nSectors;
}
uint L1InfoToStorage::getL1KTAGNSectorsCHOD() {
	return nKTAGSectorsCHOD_;
}
void L1InfoToStorage::setL1KTAGNSectorsCHOD(uint nSectors) {
	nKTAGSectorsCHOD_ = nSectors;
}

uint L1InfoToStorage::getL1LAVNHits() {
	return nLAVHits_;
}
void L1InfoToStorage::setL1LAVNHits(uint nHits) {
	nLAVHits_ = nHits;
}

uint L1InfoToStorage::getL1IRCSACNHits() {
	return nIRCSACHits_;
}
void L1InfoToStorage::setL1IRCSACNHits(uint nHits) {
	nIRCSACHits_ = nHits;
}

uint L1InfoToStorage::getL1StrawNTracks() {
	return nStrawTracks_;
}
void L1InfoToStorage::setL1StrawNTracks(uint nTracks) {
	nStrawTracks_ = nTracks;
}
double L1InfoToStorage::getL1StrawTrackP(uint iTrack) {
	return trackP_[iTrack];
}
void L1InfoToStorage::setL1StrawTrackP(uint iTrack, double momentum) {
	trackP_[iTrack] = momentum;
}
double L1InfoToStorage::getL1StrawTrackVz(uint iTrack) {
	return trackVz_[iTrack];
}
void L1InfoToStorage::setL1StrawTrackVz(uint iTrack, double vertex) {
	trackVz_[iTrack] = vertex;
}

uint L1InfoToStorage::getL1MUV3NTiles() {
	return nMUV3Tiles_;
}
void L1InfoToStorage::setL1MUV3NTiles(uint nTiles) {
	nMUV3Tiles_ = nTiles;
}

double L1InfoToStorage::getNewCHODAverageTime() {
	return newchodTime_;
}
void L1InfoToStorage::setNewCHODAverageTime(double time) {
	newchodTime_ = time;
}
uint L1InfoToStorage::getL1NewCHODNHits() {
	return nNewCHODHits_;
}
void L1InfoToStorage::setL1NewCHODNHits(uint nHits) {
	nNewCHODHits_ = nHits;
}


