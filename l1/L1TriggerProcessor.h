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
#include <atomic>
#include <cstdint>
#include <iostream>

#include "StrawAlgorithm/StrawL1Algorithm.h"

namespace na62 {

class EventBuilder;
class Event;

class L1TriggerProcessor {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint16_t <0> if the event is rejected, the L0 and L1 trigger type word (L0 + L1<<8) in other cases. The L0
	 * 								   trigger type word should be taken from the L0TP subevent
	 */
	static uint16_t compute(Event* event);
	
	static void Initialize(const uint L1_DOWNSCALE_FACTOR) {
		L1TriggerProcessor::L1_DOWNSCALE_FACTOR = L1_DOWNSCALE_FACTOR;
	}

	static std::atomic<int> rr;
	static uint L1_DOWNSCALE_FACTOR;
};

} /* namespace na62 */
#endif /* TRIGGERPROCESSOR_H_ */

/*
 * Defines the structure of a L0 event header within a MEP as defined in table 2 in NA62-11-02.
 * The data is over-allocated so you can access the actual payload by something like
 * char* data = MEPFragmentHdr_ptr+sizeof(MEPFragment_HDR);
 
 struct MEPFragment_HDR {
	uint16_t eventLength_; // Number of bytes of the following event data,	including this header.
	uint8_t eventNumberLSB_;
	uint8_t reserved_ :7;
	uint8_t lastEventOfBurst_ :1; // don't take bool as it will allocate 8 bits!
	uint32_t timestamp_;
}__attribute__ ((__packed__));

*/
