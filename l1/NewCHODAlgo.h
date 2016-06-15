/*
 * NewCHODAlgo.h
 *
 *  Created on: 16 May 2016
 *      Author: lorenzaiacobuzio
 */

#pragma once
#ifndef NEWCHODALGO_H_
#define NEWCHODALGO_H_


#include <sys/types.h>
#include <cstdint>

#include "../common/decoding/DecoderHandler.h"
#include "newchod_algorithm/ParsConfFile.h"
#include "L1InfoToStorage.h"
#include "../struct/HLTConfParams.h"
#include "L1Fragment.h"

namespace na62 {

class NewCHODAlgo {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	NewCHODAlgo();
	~NewCHODAlgo();

	static uint_fast8_t processNewCHODTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info);
//	static void initialize(uint i, l1NewCHOD &l1NewChodStruct);
	static void writeData(L1Algo* algoPacket, uint l0MaskID);

	static bool isAlgoProcessed();
	static void resetAlgoProcessed();
	static bool isEmptyPacket();
	static bool isBadData();
	static void clear();

private:

	static NewCHODParsConfFile* infoNewCHOD_;
	static uint algoID; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
	static uint algoLogic[16];
	static uint algoRefTimeSourceID[16];
	static double algoOnlineTimeWindow[16];

	static bool algoProcessed;
	static bool emptyPacket;
	static bool badData;
	static bool isNewCHODRefTime;

	static int* PMTGeo;
	static uint nHits;
	static uint nMaxPMTs;
	static int PMTID1;
	static int PMTID2;
	static double averageHitTime;
};

} /* namespace na62 */



#endif /* NEWCHODALGO_H_ */
