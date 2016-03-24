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

#define maxNhits 500

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
	static uint_fast8_t processMUV3Trigger(DecoderHandler& decoder);

private:

	static MUV3ParsConfFile* infoMUV3_;
	static int* pmtGeo;
	static uint nHits;
	static uint nMaxPMTs;
	static int pmtID;
};

} /* namespace na62 */




#endif /* MUVALGO_H_ */
