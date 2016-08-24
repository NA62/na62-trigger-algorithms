/*
 * L1InfoToStorage.h
 *
 *  Created on: 2 Nov 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#pragma once
#ifndef L1_INFO_TO_STORAGE_H_
#define L1_INFO_TO_STORAGE_H_

#include <iostream>
#include <vector>
#include <string>

//using namespace std;

class L1InfoToStorage {
public:
	L1InfoToStorage();
	~L1InfoToStorage();

	uint_fast8_t getL1CHODTrgWrd();
	void setL1CHODTrgWrd(uint_fast8_t trigger);

	uint_fast8_t getL1KTAGTrgWrd();
	void setL1KTAGTrgWrd(uint_fast8_t trigger);

	uint_fast8_t getL1LAVTrgWrd();
	void setL1LAVTrgWrd(uint_fast8_t trigger);

	uint_fast8_t getL1IRCSACTrgWrd();
	void setL1IRCSACTrgWrd(uint_fast8_t trigger);

	uint_fast8_t getL1MUV3TrgWrd();
	void setL1MUV3TrgWrd(uint_fast8_t trigger);

	uint_fast8_t getL1NewCHODTrgWrd();
	void setL1NewCHODTrgWrd(uint_fast8_t trigger);

	uint_fast8_t getL1StrawTrgWrd();
	void setL1StrawTrgWrd(uint_fast8_t trigger);

	uint_fast8_t getL1RefTimeL0TP();
	void setL1RefTimeL0TP(uint_fast8_t time);

	double getCHODAverageTime();
	void setCHODAverageTime(double time);

	double getNewCHODAverageTime();
	void setNewCHODAverageTime(double time);

	uint getL1CHODNHits();
	void setL1CHODNHits(uint nHits);

	uint getL1KTAGNSectorsL0TP();
	void setL1KTAGNSectorsL0TP(uint nSectors);

	uint getL1KTAGNSectorsCHOD();
	void setL1KTAGNSectorsCHOD(uint nSectors);

	uint getL1LAVNHits();
	void setL1LAVNHits(uint nHits);

	uint getL1IRCSACNHits();
	void setL1IRCSACNHits(uint nHits);

	uint getL1MUV3NTiles();
	void setL1MUV3NTiles(uint nTiles);

	uint getL1NewCHODNHits();
	void setL1NewCHODNHits(uint nHits);

	uint getL1StrawNTracks();
	void setL1StrawNTracks(uint nTracks);

	double getL1StrawTrackP(uint iTrack);
	void setL1StrawTrackP(uint iTrack, double momentum);

	double getL1StrawTrackVz(uint iTrackk);
	void setL1StrawTrackVz(uint iTrack, double vertex);

	void setL1CHODProcessed() {
		l1CHODProcessed_ = true;
	}
	bool isL1CHODProcessed() const {
		return l1CHODProcessed_;
	}

	void setL1CHODEmptyPacket() {
		l1CHODEmptyPacket_ = true;
	}
	bool isL1CHODEmptyPacket() const {
		return l1CHODEmptyPacket_;
	}

	void setL1CHODBadData() {
		l1CHODBadData_ = true;
	}
	bool isL1CHODBadData() const {
		return l1CHODBadData_;
	}

	void setL1NewCHODProcessed() {
		l1NewCHODProcessed_ = true;
	}
	bool isL1NewCHODProcessed() const {
		return l1NewCHODProcessed_;
	}

	void setL1NewCHODEmptyPacket() {
		l1NewCHODEmptyPacket_ = true;
	}
	bool isL1NewCHODEmptyPacket() const {
		return l1NewCHODEmptyPacket_;
	}

	void setL1NewCHODBadData() {
		l1NewCHODBadData_ = true;
	}
	bool isL1NewCHODBadData() const {
		return l1NewCHODBadData_;
	}

	void setL1KTAGProcessed() {
		l1KTAGProcessed_ = true;
	}
	bool isL1KTAGProcessed() const {
		return l1KTAGProcessed_;
	}

	void setL1KTAGEmptyPacket() {
		l1KTAGEmptyPacket_ = true;
	}
	bool isL1KTAGEmptyPacket() const {
		return l1KTAGEmptyPacket_;
	}

	void setL1KTAGBadData() {
		l1KTAGBadData_ = true;
	}
	bool isL1KTAGBadData() const {
		return l1KTAGBadData_;
	}

	void setL1LAVProcessed() {
		l1LAVProcessed_ = true;
	}
	bool isL1LAVProcessed() const {
		return l1LAVProcessed_;
	}

	void setL1LAVEmptyPacket() {
		l1LAVEmptyPacket_ = true;
	}
	bool isL1LAVEmptyPacket() const {
		return l1LAVEmptyPacket_;
	}

	void setL1LAVBadData() {
		l1LAVBadData_ = true;
	}
	bool isL1LAVBadData() const {
		return l1LAVBadData_;
	}

	void setL1IRCSACProcessed() {
		l1IRCSACProcessed_ = true;
	}
	bool isL1IRCSACProcessed() const {
		return l1IRCSACProcessed_;
	}

	void setL1IRCSACEmptyPacket() {
		l1IRCSACEmptyPacket_ = true;
	}
	bool isL1IRCSACEmptyPacket() const {
		return l1IRCSACEmptyPacket_;
	}

	void setL1IRCSACBadData() {
		l1IRCSACBadData_ = true;
	}
	bool isL1IRCSACBadData() const {
		return l1IRCSACBadData_;
	}

	void setL1MUV3Processed() {
		l1MUV3Processed_ = true;
	}
	bool isL1MUV3Processed() const {
		return l1MUV3Processed_;
	}

	void setL1MUV3EmptyPacket() {
		l1MUV3EmptyPacket_ = true;
	}
	bool isL1MUV3EmptyPacket() const {
		return l1MUV3EmptyPacket_;
	}

	void setL1MUV3BadData() {
		l1MUV3BadData_ = true;
	}
	bool isL1MUV3BadData() const {
		return l1MUV3BadData_;
	}

	void setL1StrawProcessed() {
		l1StrawProcessed_ = true;
	}
	bool isL1StrawProcessed() const {
		return l1StrawProcessed_;
	}

	void setL1StrawEmptyPacket() {
		l1StrawEmptyPacket_ = true;
	}
	bool isL1StrawEmptyPacket() const {
		return l1StrawEmptyPacket_;
	}

	void setL1StrawBadData() {
		l1StrawBadData_ = true;
	}
	bool isL1StrawBadData() const {
		return l1StrawBadData_;
	}

	void setL1StrawOverflow() {
		l1StrawOverflow_ = true;
	}
	bool isL1StrawOverflow() {
		return l1StrawOverflow_;
	}

private:

	double chodTime_;
	double newchodTime_;
	uint_fast8_t refTimeL0TP_;

	uint_fast8_t l1CHODTrgWrd_;
	uint_fast8_t l1KTAGTrgWrd_;
	uint_fast8_t l1LAVTrgWrd_;
	uint_fast8_t l1IRCSACTrgWrd_;
	uint_fast8_t l1StrawTrgWrd_;
	uint_fast8_t l1MUV3TrgWrd_;
	uint_fast8_t l1NewCHODTrgWrd_;

	bool l1CHODProcessed_;
	bool l1KTAGProcessed_;
	bool l1LAVProcessed_;
	bool l1IRCSACProcessed_;
	bool l1StrawProcessed_;
	bool l1MUV3Processed_;
	bool l1NewCHODProcessed_;

	bool l1CHODEmptyPacket_;
	bool l1KTAGEmptyPacket_;
	bool l1LAVEmptyPacket_;
	bool l1IRCSACEmptyPacket_;
	bool l1StrawEmptyPacket_;
	bool l1MUV3EmptyPacket_;
	bool l1NewCHODEmptyPacket_;

	bool l1CHODBadData_;
	bool l1KTAGBadData_;
	bool l1LAVBadData_;
	bool l1IRCSACBadData_;
	bool l1StrawBadData_;
	bool l1MUV3BadData_;
	bool l1NewCHODBadData_;

	bool l1StrawOverflow_;

	uint nCHODHits_;
	uint nKTAGSectorsL0TP_;
	uint nKTAGSectorsCHOD_;
	uint nLAVHits_;
	uint nIRCSACHits_;
	uint nStrawTracks_;
	uint nMUV3Tiles_;
	uint nNewCHODHits_;

	double trackP_[5];
	double trackVz_[5];

};

#endif /* L1_INFO_TO_STORAGE_H_ */

