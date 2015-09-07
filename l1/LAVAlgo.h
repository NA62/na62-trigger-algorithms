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

#define maxNhits 500

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
	static uint_fast8_t processLAVTrigger(DecoderHandler& decoder);

private:

	static LAVParsConfFile* infoLAV_;
	static int* lgGeo;
	static uint nHits;

};

} /* namespace na62 */

#endif /* L1_LAVALGO_H_ */
