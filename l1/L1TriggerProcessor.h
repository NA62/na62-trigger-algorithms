/*
 * TriggerProcessor.h
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#pragma once
#ifndef TRIGGERPROCESSOR_H_
#define TRIGGERPROCESSOR_H_

#include <sys/types.h>
#include <cstdint>

namespace na62 {

class EventBuilder;
class Event;

class L1TriggerProcessor {
public:
	L1TriggerProcessor();
	virtual ~L1TriggerProcessor();

	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint16_t <0> if the event is rejected, the L0 and L1 trigger type word (L0 + L1<<8) in other cases. The L0
	 * 								   trigger type word should be taken from the L0TP subevent
	 */
	uint16_t compute(Event* event);

	static void Initialize(const uint L1_DOWNSCALE_FACTOR) {
		L1TriggerProcessor::L1_DOWNSCALE_FACTOR = L1_DOWNSCALE_FACTOR;
	}

	int rr;
	static uint L1_DOWNSCALE_FACTOR;
};

} /* namespace na62 */
#endif /* TRIGGERPROCESSOR_H_ */
