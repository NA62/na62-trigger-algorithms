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
#include <cstdint>
#include <vector>
#include <random>
#include <atomic>

#include "../options/TriggerOptions.h"
#include "../struct/HLTConfParams.h"

#include "L2Fragment.h"

namespace na62 {

class EventBuilder;
class Event;

class L2TriggerProcessor {
public:
	/**
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L2 trigger type word in other cases
	 */
	static uint_fast8_t compute(Event* event);

	/**
	 * Will send a unicast request to all CREAMs within the given localCREAMIDs vector
	 */
	static void async_requestNonZSuppressedLKrData(
			const std::vector<uint_fast16_t> crateCREAMIDs, Event* event);

	/**
	 * Will be executed as soon as all non zero suppressed LKr events are received but only
	 * if async_requestNonZSuppressedLKrData has been called
	 * @return uint_fast8_t <0> if the event is rejected, the L2 trigger type word in other cases
	 */
	static uint_fast8_t onNonZSuppressedLKrDataReceived(Event* event);

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
	static void initialize(l2Struct &l2Struct);

	static inline std::atomic<uint64_t>* GetL2TriggerStats() {
		return L2Triggers_;
	}
	static inline uint64_t GetL2InputStats() {
		return L2InputEvents_;
	}
	static inline uint64_t GetL2InputReducedStats() {
		return L2InputReducedEvents_;
	}
	static inline uint64_t GetL2InputEventsPerBurst() {
		return L2InputEventsPerBurst_;
	}
	static void ResetL2InputEventsPerBurst() {
		L2InputEventsPerBurst_ = 0;
	}
	static inline uint GetL2ReductionFactor() {
		return reductionFactor;
	}
	static inline uint GetL2DownscaleFactor() {
		return downscaleFactor;
	}
	static inline uint GetL2FlagMode() {
		return flagMode;
	}
	static inline uint GetL2AutoFlagFactor() {
		return autoFlagFactor;
	}

	/**
	 * Fills output L2 structures for merger
	 */
	static void writeData(L2Block &l2Block);

private:
	static std::atomic<uint64_t>* L2Triggers_;
	static std::atomic<uint64_t> L2InputEvents_;
	static std::atomic<uint64_t> L2InputReducedEvents_;
	static std::atomic<uint64_t> L2InputEventsPerBurst_;
	static std::atomic<uint64_t> L2AcceptedEvents_;

	static uint numberOfEnabledAlgos[16];
	static uint numberOfFlaggedAlgos[16];
	static uint maskReductionFactor[16];

	static uint_fast16_t algoEnableMask[16];
	static uint_fast16_t algoFlagMask[16];
	static uint_fast16_t algoLogicMask[16];
	static uint_fast16_t algoDwScMask[16];

	static double bypassProbability;
	static uint reductionFactor;
	static uint downscaleFactor;
	static uint flagMode;
	static uint autoFlagFactor;
	static uint referenceTimeSourceID;

	static bool isL0PhysicsTrigger;
	static bool isL0PeriodicTrigger;
	static bool isL0ControlTrigger;
	static bool isL2Bypassed;
	static uint numberOfTriggeredL2Masks;
	static bool isAllL2AlgoDisable;

	static uint_fast8_t l0TrigWord;
	static uint_fast8_t l0DataType; //0x1 for physics, 0x2 for periodics, 0x4 for calibrations
	static uint_fast16_t l0TrigFlags;
	static uint_fast8_t l2TriggerWords[16];

	static uint MaskIDToNum[16];
	static uint NumToMaskID[16];

	static uint_fast8_t numberOfEnabledL0Masks;

	static std::vector<int> l0MaskIDs;
};

} /* namespace na62 */
#endif /* L2TriggerProcessor_H_ */
