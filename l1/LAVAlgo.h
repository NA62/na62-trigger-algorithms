/*
 * LAVAlgo.h
 *
 *  Created on: 7 Sep 2015
 *      Author: romano
 */

#pragma once
#ifndef L1_LAVALGO_H_
#define L1_LAVALGO_H_

#include <sys/types.h>
#include <cstdint>

#include "../common/decoding/DecoderHandler.h"
#include "lav_algorithm/ParsConfFile.h"
#include "L1InfoToStorage.h"
#include "../struct/HLTConfParams.h"
#include "L1Fragment.h"

#define maxNhits 500
#define maxNROchs 512

namespace na62 {

class LAVAlgo {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	LAVAlgo();
	~LAVAlgo();

	static uint_fast8_t processLAVTrigger(DecoderHandler& decoder,L1InfoToStorage* l1Info);

	static void initialize(l1LAV &l1LAVStruct);
	static void writeData(L1Block &l1Block);

	static bool isAlgoProcessed();
	static void resetAlgoProcessed();
	static bool isEmptyPacket();
	static bool isBadData();

private:

	static LAVParsConfFile* infoLAV_;
	static uint algoID; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for MUV3, 5 for Straw
	static uint algoLogic;
	static uint algoRefTimeSourceID;
	static double algoOnlineTimeWindow;

	static bool algoProcessed;
	static bool emptyPacket;
	static bool badData;
	static bool isCHODRefTime;

	static int* lgGeo;
	static int hit[maxNROchs];
	static uint nHits;
	static double averageCHODHitTime;

};

} /* namespace na62 */

#endif /* L1_LAVALGO_H_ */
