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

	double getCHODAverageTime();
	void setCHODAverageTime(double time);

	double getNewCHODAverageTime();
	void setNewCHODAverageTime(double time);

	uint getL1CHODNHits();
	void setL1CHODNHits(uint nHits);

	uint getL1NewCHODNHits();
	void setL1NewCHODNHits(uint nHits);

	uint getL1KTAGNSectors_l0tp();
	void setL1KTAGNSectors_l0tp(uint nSectors);

	uint getL1KTAGNSectors_chod();
	void setL1KTAGNSectors_chod(uint nSectors);

	uint getL1LAVNHits();
	void setL1LAVNHits(uint nHits);

	uint getL1MUV3NTiles();
	void setL1MUV3NTiles(uint nTiles);

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

private:

	double chodTime;
	double newchodTime;

	bool l1CHODProcessed_;
	bool l1KTAGProcessed_;
	bool l1LAVProcessed_;
	bool l1MUV3Processed_;
	bool l1NewCHODProcessed_;

	bool l1CHODEmptyPacket_;
	bool l1KTAGEmptyPacket_;
	bool l1LAVEmptyPacket_;
	bool l1MUV3EmptyPacket_;
	bool l1NewCHODEmptyPacket_;

	bool l1CHODBadData_;
	bool l1KTAGBadData_;
	bool l1LAVBadData_;
	bool l1MUV3BadData_;
	bool l1NewCHODBadData_;

	uint nKTAGSectors_l0tp;
	uint nKTAGSectors_chod;
	uint nCHODHits;
	uint nLAVHits;
	uint nMUV3Tiles;
	uint nNewCHODHits;

};

#endif /* L1_INFO_TO_STORAGE_H_ */

