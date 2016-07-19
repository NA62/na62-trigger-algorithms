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
	l1CHODTrgWrd_ = 0;
	l1KTAGTrgWrd_ = 0;
	l1LAVTrgWrd_ = 0;
	l1IRCSACTrgWrd_ = 0;
	l1MUV3TrgWrd_ = 0;
	l1NewCHODTrgWrd_ = 0;
	l1StrawTrgWrd_ = 0;

	chodTime = 0.;
	nKTAGSectors_l0tp = 0;
	nKTAGSectors_chod = 0;
	nCHODHits = 0;
	nLAVHits = 0;
	nIRCSACHits = 0;
	nMUV3Tiles = 0;
	nNewCHODHits = 0;
	nStrawTracks = 0;

	for (uint iTrk = 0; iTrk != 5; iTrk++) {
		track_p[iTrk] = 0.;
		track_vz[iTrk] = 0.;
	}

	l1CHODProcessed_ = false;
	l1CHODEmptyPacket_ = false;
	l1CHODBadData_ = false;
	l1KTAGProcessed_ = false;
	l1KTAGEmptyPacket_ = false;
	l1KTAGBadData_ = false;
	l1LAVProcessed_ = false;
	l1LAVEmptyPacket_ = false;
	l1LAVBadData_ = false;
	l1StrawProcessed_ = false;
	l1StrawEmptyPacket_ = false;
	l1StrawBadData_ = false;
	l1IRCSACProcessed_ = false;
	l1IRCSACEmptyPacket_ = false;
	l1IRCSACBadData_ = false;
	l1MUV3Processed_ = false;
	l1MUV3EmptyPacket_ = false;
	l1MUV3BadData_ = false;
	l1NewCHODProcessed_ = false;
	l1NewCHODEmptyPacket_ = false;
	l1NewCHODBadData_ = false;
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

uint_fast8_t L1InfoToStorage::getL1StrawTrgWrd() {
	return l1StrawTrgWrd_;
}
void L1InfoToStorage::setL1StrawTrgWrd(uint_fast8_t trigger) {
	l1StrawTrgWrd_ = trigger;
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

uint L1InfoToStorage::getL1IRCSACNHits() {
	return nIRCSACHits;
}
void L1InfoToStorage::setL1IRCSACNHits(uint nHits) {
	nIRCSACHits = nHits;
}

uint L1InfoToStorage::getL1MUV3NTiles() {
	return nMUV3Tiles;
}
void L1InfoToStorage::setL1MUV3NTiles(uint nTiles) {
	nMUV3Tiles = nTiles;
}

double L1InfoToStorage::getNewCHODAverageTime() {
	return newchodTime;
}
void L1InfoToStorage::setNewCHODAverageTime(double time) {
	newchodTime = time;
}
uint L1InfoToStorage::getL1NewCHODNHits() {
	return nNewCHODHits;
}
void L1InfoToStorage::setL1NewCHODNHits(uint nHits) {
	nNewCHODHits = nHits;
}

uint L1InfoToStorage::getL1StrawNTracks() {
	return nStrawTracks;
}
void L1InfoToStorage::setL1StrawNTracks(uint nTracks) {
	nStrawTracks = nTracks;
}
double L1InfoToStorage::getL1StrawTrack_P(uint iTrack) {
	return track_p[iTrack];
}
void L1InfoToStorage::setL1StrawTrack_P(uint iTrack, double momentum) {
	track_p[iTrack] = momentum;
}
double L1InfoToStorage::getL1StrawTrack_Vz(uint iTrack) {
	return track_vz[iTrack];
}
void L1InfoToStorage::setL1StrawTrack_Vz(uint iTrack, double vertex) {
	track_vz[iTrack] = vertex;
}

