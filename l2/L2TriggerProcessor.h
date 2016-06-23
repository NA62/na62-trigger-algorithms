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

#include "eventBuilding/Event.h"
#include "../options/TriggerOptions.h"
#include "../struct/HLTConfParams.h"

#include "L2Fragment.h"

namespace na62 {

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
		if (BypassProbability_ == 0.0) {
			return false;
		}
		double randomNr = ((double) rand() / (double) RAND_MAX);
		return randomNr <= BypassProbability_;
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
		return ReductionFactor_;
	}
	static inline uint GetL2DownscaleFactor() {
		return DownscaleFactor_;
	}
	static inline uint GetL2FlagMode() {
		return FlagMode_;
	}
	static inline uint GetL2AutoFlagFactor() {
		return AutoFlagFactor_;
	}

	/**
	 * Fills output L2 structures for merger
	 */
	static void writeData(Event* event, const uint32_t& l2TriggerWord);
	static inline uint_fast32_t GetL2DataPacketSize() {
		return L2DataPacketSize_;
	}
private:
	static std::atomic<uint64_t>* L2Triggers_;
	static std::atomic<uint64_t> L2InputEvents_;
	static std::atomic<uint64_t> L2InputReducedEvents_;
	static std::atomic<uint64_t> L2InputEventsPerBurst_;
	static std::atomic<uint64_t> L2AcceptedEvents_;

	static uint NumberOfEnabledAlgos_[16];
	static uint NumberOfFlaggedAlgos_[16];
	static uint MaskReductionFactor_[16];

	static uint_fast16_t AlgoEnableMask_[16];
	static uint_fast16_t AlgoFlagMask_[16];
	static uint_fast16_t AlgoLogicMask_[16];
	static uint_fast16_t AlgoDwScMask_[16];

	static double BypassProbability_;
	static uint ReductionFactor_;
	static uint DownscaleFactor_;
	static uint FlagMode_;
	static uint AutoFlagFactor_;
	static uint ReferenceTimeSourceID_;

	static uint MaskIDToNum_[16];
	static uint NumToMaskID_[16];

	static uint_fast8_t NumberOfEnabledL0Masks_;

	static std::vector<int> L0MaskIDs_;
	static uint_fast32_t L2DataPacketSize_;
};

} /* namespace na62 */
#endif /* L2TriggerProcessor_H_ */
