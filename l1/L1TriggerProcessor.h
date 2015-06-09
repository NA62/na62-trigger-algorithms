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
//class CedarData;
class KtagAlgo;
class RICHAlgo;
class CHODAlgo;

class L1TriggerProcessor {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	static uint_fast8_t compute(Event* const event);

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

	/**
	 * Placeholder for deciding whether or not to request ZS CREAM data
	 */
	static bool isRequestZeroSuppressedCreamData(
			uint_fast8_t l1TriggerTypeWord);

	static void initialize(double _bypassProbability);

private:
	static double bypassProbability;

	static uint cedarAlgorithmId;
	static uint chodAlgorithmId;
	static uint richAlgorithmId;
};

} /* namespace na62 */
#endif /* TRIGGERPROCESSOR_H_ */
