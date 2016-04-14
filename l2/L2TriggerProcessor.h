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

	static void initialize() {
		bypassProbability = TriggerOptions::GetDouble(
		OPTION_L2_BYPASS_PROBABILITY);

		for (int i = 0; i != 0xFF + 1; i++) {
			L2Triggers_[i] = 0;
		}

		L2ProcessingTimeVsEvtNumber_ = new std::atomic<uint64_t>*[0x64 + 1];
		for (int i = 0; i < 0x64 + 1; i++) {
			L2ProcessingTimeVsEvtNumber_[i] =
					new std::atomic<uint64_t>[0x64 + 1] { };
		}
		ResetL2ProcessingTimeVsEvtNumber();
		l2ReductionFactor = Options::GetInt(OPTION_L2_REDUCTION_FACTOR);
		l2DownscaleFactor = Options::GetInt(OPTION_L2_DOWNSCALE_FACTOR);
		l2FlagMode = Options::GetInt(OPTION_L2_FLAG_MODE);
		l2AutoFlagFactor = Options::GetInt(OPTION_L2_AUTOFLAG_FACTOR);
	}
	static inline std::atomic<uint64_t>* GetL2TriggerStats() {
		return L2Triggers_;
	}
	static inline uint64_t GetL2InputStats() {
		return L2InputEvents_;
	}
	static inline uint64_t GetL2InputReducedStats() {
		return L2InputReducedEvents_;
	}
	static inline uint64_t GetL2BypassedEvents() {
		return L2BypassedEvents_;
	}
	static inline std::atomic<uint64_t>** GetL2ProcessingTimeVsEvtNumber() {
		return L2ProcessingTimeVsEvtNumber_;
	}
	static void ResetL2ProcessingTimeVsEvtNumber() {
		for (int i = 0; i < 0x64 + 1; ++i) {
			for (int j = 0; j < 0x64 + 1; ++j) {
				L2ProcessingTimeVsEvtNumber_[i][j] = 0;
			}
		}
	}
	static inline uint64_t GetL2ProcessingTimeCumulative() {
		return L2ProcessingTimeCumulative_;
	}
	static void ResetL2ProcessingTimeCumulative() {
		L2ProcessingTimeCumulative_ = 0;
	}
	static inline uint64_t GetL2InputEventsPerBurst() {
		return L2InputEventsPerBurst_;
	}
	static void ResetL2InputEventsPerBurst() {
		L2InputEventsPerBurst_ = 0;
	}
	static inline uint64_t GetL2ProcessingTimeMax() {
		return L2ProcessingTimeMax_;
	}
	static void ResetL2ProcessingTimeMax() {
		L2ProcessingTimeMax_ = 0;
	}
	static inline uint GetL2ReductionFactor() {
		return l2ReductionFactor;
	}
	static inline uint GetL2DownscaleFactor() {
		return l2DownscaleFactor;
	}
	static inline uint GetL2FlagMode() {
		return l2FlagMode;
	}
	static inline uint GetL2AutoFlagFactor() {
		return l2AutoFlagFactor;
	}

private:
	static std::atomic<uint64_t>* L2Triggers_;
	static std::atomic<uint64_t> L2BypassedEvents_;
	static std::atomic<uint64_t> L2InputEvents_;
	static std::atomic<uint64_t> L2InputReducedEvents_;
	static std::atomic<uint64_t> L2InputEventsPerBurst_;
	static std::atomic<uint64_t> L2AcceptedEvents_;
	static std::atomic<uint64_t> L2ProcessingTimeCumulative_;
	static std::atomic<uint64_t> L2ProcessingTimeMax_;
	static std::atomic<uint64_t>** L2ProcessingTimeVsEvtNumber_;

	static uint l2ReductionFactor;
	static uint l2DownscaleFactor;
	static uint l2FlagMode;
	static uint l2AutoFlagFactor;
	static double bypassProbability;
};

} /* namespace na62 */
#endif /* L2TriggerProcessor_H_ */
