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
#include <random>
#include <cstdint>

namespace na62 {

class EventBuilder;
class Event;
class CedarData;
class KtagAlgo;

class L1TriggerProcessor {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	static uint8_t compute(Event* event);

	/**
	 * Returns true if the current event should be bypassed instead of being processed
	 */
	static inline bool bypassEvent() {
		if (bypassProbability == 0.0) {
			return false;
		}
		double randomNr = ((double) rand() / (double) RAND_MAX);
		return randomNr <= bypassProbability;
	}

	/**
	 * Registers all downscaling algorithms. Must be called before Options::Load is executed!
	 */
	static void registerDownscalingAlgorithms();

	static void initialize(double _bypassProbability);

private:
	static double bypassProbability;

	static uint cedarAlgorithmId;
};

} /* namespace na62 */
#endif /* TRIGGERPROCESSOR_H_ */
