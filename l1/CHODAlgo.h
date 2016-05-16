/*
 * CHODAlgo.h
 *
 *  Created on: May 11, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

#pragma once
#ifndef CHODALGORITHM_H_
#define CHODALGORITHM_H_

#include <sys/types.h>
#include <cstdint>

#include "../common/decoding/DecoderHandler.h"
#include "chod_algorithm/ParsConfFile.h"
#include "L1InfoToStorage.h"
#include "../struct/HLTConfParams.h"
#include "L1Fragment.h"

#define maxNhits 500

namespace na62 {

class CHODAlgo {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	CHODAlgo();
	~CHODAlgo();

	static uint_fast8_t processCHODTrigger(DecoderHandler& decoder, L1InfoToStorage* l1Info);
	static void initialize(l1CHOD &l1ChodStruct);
	static void writeData(L1Block &l1Block);

	static bool isAlgoProcessed();
	static void resetAlgoProcessed();
	static bool isEmptyPacket();
	static bool isBadData();

private:

	static CHODParsConfFile* infoCHOD_;
	static uint algoID; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for MUV3, 5 for Straw
	static uint algoLogic;
	static uint algoRefTimeSourceID;
	static double algoOnlineTimeWindow;

	static bool algoProcessed;
	static bool emptyPacket;
	static bool badData;
	static bool isCHODRefTime;

	static int* slabGeo;
	static uint nHits_V, nHits_H;
	static uint nMaxSlabs;
	static int slabID;
//	static int quadrantID;
	static int planeID;
	static double averageHitTime;

};

} /* namespace na62 */
#endif /* CHODALGORITHM_H_ */
