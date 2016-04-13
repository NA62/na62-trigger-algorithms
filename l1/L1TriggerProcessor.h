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
#include <atomic>

#include "L1InfoToStorage.h"
#include "../options/TriggerOptions.h"
#include "L1Downscaling.h"
#include "L1Reduction.h"

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
	 * Registers all reduction algorithms. Must be called before Options::Load is executed!
	 */
	static void registerReductionAlgorithms();

	/**
	 * Placeholder for deciding whether or not to request ZS CREAM data
	 */
	static bool isRequestZeroSuppressedCreamData(
			uint_fast8_t l1TriggerTypeWord);

	static inline std::atomic<uint64_t>* GetL1TriggerStats() {
		return L1Triggers_;
	}
	static inline std::atomic<uint64_t>** GetL1ProcessingTimeVsEvtNumber() {
		return L1ProcessingTimeVsEvtNumber_;
	}
	static void ResetL1ProcessingTimeVsEvtNumber() {
		for (int i = 0; i < 0x64 + 1; ++i) {
			for (int j = 0; j < 0x64 + 1; ++j) {
				L1ProcessingTimeVsEvtNumber_[i][j] = 0;
			}
		}
	}
	static inline uint64_t GetL1InputStats() {
		return L1InputEvents_;
	}
	static inline uint64_t GetL1InputReceivedStats() {
		return L1InputReducedEvents_;
	}
	static inline uint64_t GetL1Requests() {
		return L1Requests_;
	}
	static inline uint64_t GetL1ProcessingTimeCumulative() {
		return L1ProcessingTimeCumulative_;
	}
	static void ResetL1ProcessingTimeCumulative() {
		L1ProcessingTimeCumulative_ = 0;
	}
	static inline uint64_t GetL1InputEventsPerBurst() {
		return L1InputEventsPerBurst_;
	}
	static void ResetL1InputEventsPerBurst() {
		L1InputEventsPerBurst_ = 0;
	}
	static inline uint64_t GetL1ProcessingTimeMax() {
		return L1ProcessingTimeMax_;
	}
	static void ResetL1ProcessingTimeMax() {
		L1ProcessingTimeMax_ = 0;
	}
	static inline uint64_t GetL1BypassedEvents() {
		return L1BypassedEvents_;
	}
	static inline uint GetL1DownscaleFactor() {
		return downscaleFactor_;
	}
	static inline uint GetL1ReductionFactor() {
		return reductionFactor_;
	}
	static inline bool GetL1FlagMode() {
		return flagMode_;
	}
	static inline uint GetL1AutoFlagFactor() {
		return autoFlagFactor_;
	}
//	static inline uint16_t GetL1FlagMask() {
//		return l1FlagMask_;
//	}
/////////////////////////////////////////////////////////////////////////
	static void initialize() {
		// Seed for rand()
		srand(time(NULL));

		//	bypassProbability = _bypassProbability;
		bypassProbability = TriggerOptions::GetDouble(
		OPTION_L1_BYPASS_PROBABILITY);

		for (int i = 0; i != 0xFF + 1; i++) {
			L1Triggers_[i] = 0;
		}
		L1ProcessingTimeVsEvtNumber_ = new std::atomic<uint64_t>*[0x64 + 1];
		for (int i = 0; i < 0x64 + 1; i++) {
			L1ProcessingTimeVsEvtNumber_[i] =
					new std::atomic<uint64_t>[0x64 + 1] { };
		}
		ResetL1ProcessingTimeVsEvtNumber();

		reductionFactor_ = TriggerOptions::GetInt(OPTION_L1_REDUCTION_FACTOR);
		downscaleFactor_ = TriggerOptions::GetInt(OPTION_L1_DOWNSCALE_FACTOR);
		flagMode_ = (bool) TriggerOptions::GetInt(OPTION_L1_FLAG_MODE);
		autoFlagFactor_ = Options::GetInt(OPTION_L1_AUTOFLAG_FACTOR);

//		std::stringstream l1Trg;
//		l1Trg << std::hex << (TriggerOptions::GetString(OPTION_L1_TRIG_MASK));
//		l1Trg >> l1TrigMask;
//		LOG_INFO<< "l1TriggerMask " << std::hex << (uint) l1TrigMask << std::dec << ENDL;

		L1Downscaling::initialize();
		L1Reduction::initialize();
	}

private:
	static std::atomic<uint64_t>* L1Triggers_;
	static std::atomic<uint64_t> L1BypassedEvents_;
	static std::atomic<uint64_t> L1InputEvents_;
	static std::atomic<uint64_t> L1InputReducedEvents_;
	static std::atomic<uint64_t> L1InputEventsPerBurst_;
	static std::atomic<uint64_t> L1AcceptedEvents_;
	static std::atomic<uint64_t> L1Requests_;
	static std::atomic<uint64_t> L1ProcessingTimeCumulative_;
	static std::atomic<uint64_t> L1ProcessingTimeMax_;
	static std::atomic<uint64_t>** L1ProcessingTimeVsEvtNumber_;

	static uint reductionFactor_;
	static uint downscaleFactor_;
	static bool flagMode_;
	static uint autoFlagFactor_;

	static L1InfoToStorage* l1Info_;
	static double bypassProbability;
	static uint_fast8_t l0TrigWord;
	static uint_fast16_t l0TrigFlags;
	static uint_fast16_t l1TrigMask;
	static uint_fast16_t l1TrigLogic;
	static uint_fast16_t l1TrigFlag;
	static uint_fast16_t l1TrigDS;
	static uint l1ReferenceTimeSource;

	static uint cedarAlgorithmId;
	static uint chodAlgorithmId;
	static uint richAlgorithmId;
	static uint lavAlgorithmId;
}
;

} /* namespace na62 */
#endif /* TRIGGERPROCESSOR_H_ */
