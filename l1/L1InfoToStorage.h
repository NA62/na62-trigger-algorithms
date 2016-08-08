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

	double getCHODAverageTime();
	void setCHODAverageTime(double time);

	double getNewCHODAverageTime();
	void setNewCHODAverageTime(double time);

	uint getL1CHODNHits();
	void setL1CHODNHits(uint nHits);

	uint getL1KTAGNSectors_l0tp();
	void setL1KTAGNSectors_l0tp(uint nSectors);

	uint getL1KTAGNSectors_chod();
	void setL1KTAGNSectors_chod(uint nSectors);

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

	double getL1StrawTrack_P(uint iTrack);
	void setL1StrawTrack_P(uint iTrack, double momentum);

	double getL1StrawTrack_Vz(uint iTrackk);
	void setL1StrawTrack_Vz(uint iTrack, double vertex);

	void setL1CHODProcessed() {
		l1CHODProcessed_ = true;
	}
	bool isL1CHODProcessed() const {
		return l1CHODProcessed_;
	}
	void resetL1CHODProcessed() {
		l1CHODProcessed_ = false;
	}
	void setL1CHODEmptyPacket() {
		l1CHODEmptyPacket_ = true;
	}
	bool isL1CHODEmptyPacket() const {
		return l1CHODEmptyPacket_;
	}
	void resetL1CHODEmptyPacket() {
		l1CHODEmptyPacket_ = false;
	}
	void setL1CHODBadData() {
		l1CHODBadData_ = true;
	}
	bool isL1CHODBadData() const {
		return l1CHODBadData_;
	}
	void resetL1CHODBadData() {
		l1CHODBadData_ = false;
	}

	void setL1NewCHODProcessed() {
		l1NewCHODProcessed_ = true;
	}
	bool isL1NewCHODProcessed() const {
		return l1NewCHODProcessed_;
	}
	void resetL1NewCHODProcessed() {
		l1NewCHODProcessed_ = false;
	}
	void setL1NewCHODEmptyPacket() {
		l1NewCHODEmptyPacket_ = true;
	}
	bool isL1NewCHODEmptyPacket() const {
		return l1NewCHODEmptyPacket_;
	}
	void resetL1NewCHODEmptyPacket() {
		l1NewCHODEmptyPacket_ = false;
	}
	void setL1NewCHODBadData() {
		l1NewCHODBadData_ = true;
	}
	bool isL1NewCHODBadData() const {
		return l1NewCHODBadData_;
	}
	void resetL1NewCHODBadData() {
		l1NewCHODBadData_ = false;
	}

	void setL1KTAGProcessed() {
		l1KTAGProcessed_ = true;
	}
	bool isL1KTAGProcessed() const {
		return l1KTAGProcessed_;
	}
	void resetL1KTAGProcessed() {
		l1KTAGProcessed_ = false;
	}
	void setL1KTAGEmptyPacket() {
		l1KTAGEmptyPacket_ = true;
	}
	bool isL1KTAGEmptyPacket() const {
		return l1KTAGEmptyPacket_;
	}
	void resetL1KTAGEmptyPacket() {
		l1KTAGEmptyPacket_ = false;
	}
	void setL1KTAGBadData() {
		l1KTAGBadData_ = true;
	}
	bool isL1KTAGBadData() const {
		return l1KTAGBadData_;
	}
	void resetL1KTAGBadData() {
		l1KTAGBadData_ = false;
	}

	void setL1LAVProcessed() {
		l1LAVProcessed_ = true;
	}
	bool isL1LAVProcessed() const {
		return l1LAVProcessed_;
	}
	void resetL1LAVProcessed() {
		l1LAVProcessed_ = false;
	}
	void setL1LAVEmptyPacket() {
		l1LAVEmptyPacket_ = true;
	}
	bool isL1LAVEmptyPacket() const {
		return l1LAVEmptyPacket_;
	}
	void resetL1LAVEmptyPacket() {
		l1LAVEmptyPacket_ = false;
	}
	void setL1LAVBadData() {
		l1LAVBadData_ = true;
	}
	bool isL1LAVBadData() const {
		return l1LAVBadData_;
	}
	void resetL1LAVBadData() {
		l1LAVBadData_ = false;
	}

	void setL1IRCSACProcessed() {
		l1IRCSACProcessed_ = true;
	}
	bool isL1IRCSACProcessed() const {
		return l1IRCSACProcessed_;
	}
	void resetL1IRCSACProcessed() {
		l1IRCSACProcessed_ = false;
	}
	void setL1IRCSACEmptyPacket() {
		l1IRCSACEmptyPacket_ = true;
	}
	bool isL1IRCSACEmptyPacket() const {
		return l1IRCSACEmptyPacket_;
	}
	void resetL1IRCSACEmptyPacket() {
		l1IRCSACEmptyPacket_ = false;
	}
	void setL1IRCSACBadData() {
		l1IRCSACBadData_ = true;
	}
	bool isL1IRCSACBadData() const {
		return l1IRCSACBadData_;
	}
	void resetL1IRCSACBadData() {
		l1IRCSACBadData_ = false;
	}

	void setL1MUV3Processed() {
		l1MUV3Processed_ = true;
	}
	bool isL1MUV3Processed() const {
		return l1MUV3Processed_;
	}
	void resetL1MUV3Processed() {
		l1MUV3Processed_ = false;
	}
	void setL1MUV3EmptyPacket() {
		l1MUV3EmptyPacket_ = true;
	}
	bool isL1MUV3EmptyPacket() const {
		return l1MUV3EmptyPacket_;
	}
	void resetL1MUV3EmptyPacket() {
		l1MUV3EmptyPacket_ = false;
	}
	void setL1MUV3BadData() {
		l1MUV3BadData_ = true;
	}
	bool isL1MUV3BadData() const {
		return l1MUV3BadData_;
	}
	void resetL1MUV3BadData() {
		l1MUV3BadData_ = false;
	}

	void setL1StrawProcessed() {
		l1StrawProcessed_ = true;
	}
	bool isL1StrawProcessed() const {
		return l1StrawProcessed_;
	}
	void resetL1StrawProcessed() {
		l1StrawProcessed_ = false;
	}
	void setL1StrawEmptyPacket() {
		l1StrawEmptyPacket_ = true;
	}
	bool isL1StrawEmptyPacket() const {
		return l1StrawEmptyPacket_;
	}
	void resetL1StrawEmptyPacket() {
		l1StrawEmptyPacket_ = false;
	}
	void setL1StrawBadData() {
		l1StrawBadData_ = true;
	}
	bool isL1StrawBadData() const {
		return l1StrawBadData_;
	}
	void resetL1StrawBadData() {
		l1StrawBadData_ = false;
	}

	void setL1StrawOverflow() {
		l1StrawOverflow_ = true;
	}
	bool isL1StrawOverflow() {
		return l1StrawOverflow_;
	}

private:

	double chodTime;
	double newchodTime;

	uint_fast8_t l1CHODTrgWrd_;
	uint_fast8_t l1KTAGTrgWrd_;
	uint_fast8_t l1LAVTrgWrd_;
	uint_fast8_t l1IRCSACTrgWrd_;
	uint_fast8_t l1MUV3TrgWrd_;
	uint_fast8_t l1NewCHODTrgWrd_;
	uint_fast8_t l1StrawTrgWrd_;

	bool l1CHODProcessed_;
	bool l1KTAGProcessed_;
	bool l1LAVProcessed_;
	bool l1StrawProcessed_;
	bool l1IRCSACProcessed_;
	bool l1MUV3Processed_;
	bool l1NewCHODProcessed_;

	bool l1CHODEmptyPacket_;
	bool l1KTAGEmptyPacket_;
	bool l1LAVEmptyPacket_;
	bool l1StrawEmptyPacket_;
	bool l1IRCSACEmptyPacket_;
	bool l1MUV3EmptyPacket_;
	bool l1NewCHODEmptyPacket_;

	bool l1CHODBadData_;
	bool l1KTAGBadData_;
	bool l1LAVBadData_;
	bool l1StrawBadData_;
	bool l1IRCSACBadData_;
	bool l1MUV3BadData_;
	bool l1NewCHODBadData_;

	bool l1StrawOverflow_;

	uint nKTAGSectors_l0tp;
	uint nKTAGSectors_chod;
	uint nCHODHits;
	uint nLAVHits;
	uint nIRCSACHits;
	uint nMUV3Tiles;
	uint nNewCHODHits;
	uint nStrawTracks;

	double track_p[5];
	double track_vz[5];

};

#endif /* L1_INFO_TO_STORAGE_H_ */

