/*
 * MultiDetAlgo.h
 *
 *  Created on: March 05, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

#pragma once
#ifndef MULTIALGORITHM_H_
#define MULTIALGORITHM_H_

#include <cstdint>

#include "../common/decoding/DecoderHandler.h"

#define maxNhits 500

namespace na62 {


class MultiDetAlgo {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	MultiDetAlgo();
	~MultiDetAlgo();
	static uint_fast8_t processMultiDetTrigger(DecoderHandler& decoder);

private:

	static uint nHits;
	static uint nMaxSlabs;
	static uint nCandidates;
	static int chROID[maxNhits];
	static int quadrantID[maxNhits];
	static int planeID[maxNhits];
	static uint64_t time[maxNhits];


};

} /* namespace na62 */
#endif /* MULTIALGORITHM_H_ */
