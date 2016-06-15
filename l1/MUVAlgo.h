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
#include "L1Fragment.h"

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
	static uint_fast8_t processMUV3Trigger0(uint l0MaskID, DecoderHandler& decoder,
			L1InfoToStorage* l1Info);
	static uint_fast8_t processMUV3Trigger1(uint l0MaskID, DecoderHandler& decoder,
			L1InfoToStorage* l1Info);
	static uint_fast8_t processMUV3Trigger2(uint l0MaskID, DecoderHandler& decoder,
			L1InfoToStorage* l1Info);
	static void initialize(uint i,l1MUV &l1MUV3Struct);
	static void writeData(L1Algo* algoPacket, uint l0MaskID);

	static bool isAlgoProcessed();
	static void resetAlgoProcessed();
	static bool isEmptyPacket();
	static bool isBadData();
	static void clear();

private:

	static MUV3ParsConfFile* infoMUV3_;
	static uint algoID; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
	static uint algoLogic[16];
	static uint algoRefTimeSourceID[16];
	static double algoOnlineTimeWindow[16];

	static bool algoProcessed;
	static bool emptyPacket;
	static bool badData;
	static bool isCHODRefTime;
	static double averageCHODHitTime;

	static int* pmtGeo;
	static int roChID;
	static uint pmtID1;
	static uint pmtID2;

	static bool tileID[152];
	static uint pmtID[2];
	static uint nTiles;

};

} /* namespace na62 */

#endif /* MUVALGO_H_ */
