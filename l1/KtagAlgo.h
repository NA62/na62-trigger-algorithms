/*
 * KtagAlgo.h
 *
 *  Created on: Jan 25, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

#pragma once
#ifndef KTAGALGORITHM_H_
#define KTAGALGORITHM_H_

#include <sys/types.h>
#include <cstdint>

#include "../common/decoding/Decoder.h"

namespace na62 {

class Event;

class KtagAlgo {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */
	static uint_fast8_t processKtagTrigger(const Decoder& decoder);
	static uint searchPMT(uint tel62ID, uint fpgaID);

private:

};

} /* namespace na62 */
#endif /* KTAGALGORITHM_H_ */
