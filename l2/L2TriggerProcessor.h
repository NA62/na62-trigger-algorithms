/*
 * L2TriggerProcessor.h
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#pragma once
#ifndef L2TriggerProcessor_H_
#define L2TriggerProcessor_H_

#include <sys/types.h>
#include <atomic>
#include <cstdint>
#include <vector>

namespace na62 {

class EventBuilder;
class Event;

class L2TriggerProcessor {
private:
	static std::atomic<int> rr;

public:
	/**
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint8_t <0> if the event is rejected, the L2 trigger type word in other cases
	 */
	static uint8_t compute(Event* event);

	/**
	 * Will send a unicast request to all CREAMs within the given localCREAMIDs vector
	 */
	static void async_requestNonZSuppressedLKrData(
			const std::vector<uint16_t> crateCREAMIDs, Event* event);

	/**
	 * Will be executed as soon as all non zero suppressed LKr events are received but only
	 * if async_requestNonZSuppressedLKrData has been called
	 * @return uint8_t <0> if the event is rejected, the L2 trigger type word in other cases
	 */
	static uint8_t onNonZSuppressedLKrDataReceived(Event* event);

	static void Initialize(const uint L2_DOWNSCALE_FACTOR) {
		L2TriggerProcessor::L2_DOWNSCALE_FACTOR = L2_DOWNSCALE_FACTOR;
	}

	static uint L2_DOWNSCALE_FACTOR;
};

} /* namespace na62 */
#endif /* L2TriggerProcessor_H_ */
