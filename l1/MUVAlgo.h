/*
 * MUVAlgo.h
 *
 *  Created on: 24 Mar 2016
 *      Author: lorenza
 */

#ifndef MUVALGO_H_
#define MUVALGO_H_

#include <sys/types.h>
#include <cstdint>

#include "../common/decoding/DecoderHandler.h"
#include "muv_algorithm/ParsConfFile.h"
#include "L1InfoToStorage.h"
#include "../struct/HLTConfParams.h"

namespace na62 {

class MUV3Algo {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	MUV3Algo();
	~MUV3Algo();
	static uint_fast8_t processMUV3Trigger1(DecoderHandler& decoder,
			L1InfoToStorage* l1Info);
	static uint_fast8_t processMUV3Trigger2(DecoderHandler& decoder,
			L1InfoToStorage* l1Info);
	static void initialize(l1MUV &l1MUV3Struct);

	static bool isAlgoProcessed();
	static void resetAlgoProcessed();
	static bool isEmptyPacket();
	static bool isBadData();

private:

	static MUV3ParsConfFile* infoMUV3_;
	static uint algoID; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for MUV3, 5 for Straw
	static uint algoLogic;
	static uint algoRefTimeSourceID;
	static double algoOnlineTimeWindow;

	static bool algoProcessed;
	static bool emptyPacket;
	static bool badData;
	static bool isCHODRefTime;
	static double averageCHODHitTime;

	static int* pmtGeo;
	static int roChID;
	static uint pmtID1;
	static uint pmtID2;

};

} /* namespace na62 */

#endif /* MUVALGO_H_ */
