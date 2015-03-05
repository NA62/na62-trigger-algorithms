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

#include <sys/types.h>
#include <atomic>
#include <cstdint>

namespace na62 {

class Event;

class MultiDetAlgo {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	MultiDetAlgo();
	~MultiDetAlgo();
	static uint8_t checkMultiDetTrigger(Event* event);

private:

};

} /* namespace na62 */
#endif /* MULTIALGORITHM_H_ */
