/*
 * IRC_SACAlgo.h
 *
 *  Created on: 20 May 2016
 *      Author: lorenza
 */

#pragma once
#ifndef L1_IRC_SACALGO_H_
#define L1_IRC_SACALGO_H_

#include <sys/types.h>
#include <cstdint>

#include "../common/decoding/DecoderHandler.h"
#include "ircsac_algorithm/ParsConfFile.h"
#include "L1InfoToStorage.h"
#include "../struct/HLTConfParams.h"
#include "L1Fragment.h"

#define maxNhits 500
#define maxNROchs 512

namespace na62 {

class IRC_SACAlgo {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	IRC_SACAlgo();
	~IRC_SACAlgo();

	static uint_fast8_t processIRCSACTrigger(DecoderHandler& decoder,L1InfoToStorage* l1Info);

	static void initialize(l1IRCSAC &l1IRCSACStruct, uint_fast8_t nEnabledMasks);
	static void writeData(L1Block &l1Block);

	static bool isAlgoProcessed();
	static void resetAlgoProcessed();
	static bool isEmptyPacket();
	static bool isBadData();

private:

	static IRCSACParsConfFile* infoIRC_SAC_;
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
	static uint_fast8_t numberOfEnabledL0Masks;

};

} /* namespace na62 */

#endif /* L1_IRC_SACALGO_H_ */
