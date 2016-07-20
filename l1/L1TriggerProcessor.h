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

#include <eventBuilding/Event.h>
#include "L1InfoToStorage.h"
#include "../options/TriggerOptions.h"
#include "../struct/HLTConfParams.h"
#include "L1Downscaling.h"
#include "L1Reduction.h"
#include "L1Fragment.h"

namespace na62 {

class L1TriggerProcessor {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr and GTK)
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
		if (BypassProbability_ == 0.0) {
			return false;
		}
		double randomNr = ((double) rand() / (double) RAND_MAX);
		return randomNr <= BypassProbability_;
	}

	/**
	 * Fill in trigger decisions into event structure
	 */
	static void writeL1Data(Event* const event, const uint_fast8_t* l1TriggerWords, L1InfoToStorage* l1Info, bool isL1WhileTimeout = false);
	static void readL1Data(Event* const event);
	static bool writeAlgoPacket(int algoID, L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info);
	static bool writeStrawAlgoPacket(int algoID, L1StrawAlgo* strawAlgoPacket, uint l0MaskID, L1InfoToStorage* l1Info);
	static std::string algoIdToTriggerName(uint algoID);

	/**
	 * Placeholder for deciding whether or not to request ZS CREAM data
	 */
	static bool isRequestZeroSuppressedCreamData(uint_fast8_t l1TriggerTypeWord);

	static inline std::atomic<uint64_t>* GetL1TriggerStats() {
		return L1Triggers_;
	}
	static inline uint64_t GetL1InputStats() {
		return L1InputEvents_;
	}
	static inline uint64_t GetL1InputReducedStats() {
		return L1InputReducedEvents_;
	}
	static inline uint64_t GetL1InputEventsPerBurst() {
		return L1InputEventsPerBurst_;
	}
	static void ResetL1InputEventsPerBurst() {
		L1InputEventsPerBurst_ = 0;
	}
	static inline uint GetL1DownscaleFactor() {
		return DownscaleFactor_;
	}
	static inline uint GetL1ReductionFactor() {
		return ReductionFactor_;
	}
	static inline bool GetL1FlagMode() {
		return FlagMode_;
	}
	static inline uint GetL1AutoFlagFactor() {
		return AutoFlagFactor_;
	}
	static inline uint_fast8_t GetNumberOfEnabledL0Masks() {
		return NumberOfEnabledL0Masks_;
	}
	static inline uint GetL0MaskNumToMaskID(uint iNum) {
		return NumToMaskID_[iNum];
	}
	static inline uint GetL0MaskIDToMaskNum(uint iMaskID) {
		return MaskIDToNum_[iMaskID];
	}
	static inline uint GetNumberOfEnabledAlgoPerMask(uint iMaskID) {
		return NumberOfEnabledAlgos_[iMaskID];
	}
	static inline uint GetAlgoNumToAlgoID(uint iMaskID, uint iNum) {
		return NumToAlgoID_[iMaskID][iNum];
	}
	static inline uint GetAlgoIDToAlgoNum(uint iMaskID, uint iAlgoID) {
		return AlgoIDToNum_[iMaskID][iAlgoID];
	}
	static inline uint_fast32_t GetL1DataPacketSize() {
		return L1DataPacketSize_;
	}
	static inline uint64_t GetL1AcceptedEventsPerL0Mask(uint iMaskID) {
		return L1AcceptedEventsPerL0Mask_[iMaskID];
	}
	static inline uint64_t GetEventCountersByL0MaskByAlgoID(uint iMaskID, uint iAlgoID) {
		return EventCountersByL0MaskByAlgoID_[iMaskID][iAlgoID];
	}

	static void initialize(l1Struct &l1Struct);

private:
	static std::atomic<uint64_t>* L1Triggers_;
	static std::atomic<uint64_t>* L1AcceptedEventsPerL0Mask_;
	static std::atomic<uint64_t>* L1InputReducedEventsPerL0Mask_;
	static std::atomic<uint64_t>** EventCountersByL0MaskByAlgoID_;
	static std::atomic<uint64_t> L1InputEvents_;
	static std::atomic<uint64_t> L1InputReducedEvents_;
	static std::atomic<uint64_t> L1InputEventsPerBurst_;
	static std::atomic<uint64_t> L1AcceptedEvents_;

	//Global L1 configuration parameters (for all L0 masks)
	static double BypassProbability_;
	static uint ReductionFactor_;
	static uint DownscaleFactor_;
	static bool FlagMode_;
	static uint AutoFlagFactor_;
	static uint ReferenceTimeSourceID_;

	// L1 Mask configuration parameters (for 16 L0 masks)
	static uint NumberOfEnabledAlgos_[16];
	static uint NumberOfFlaggedAlgos_[16];
	static uint NumberOfEnabledAndFlaggedAlgos_[16];
	static uint MaskReductionFactor_[16];

	static uint_fast16_t AlgoEnableMask_[16];
	static uint_fast16_t AlgoEnableCutMask_[16];
	static uint_fast16_t AlgoFlagMask_[16];
	static uint_fast16_t AlgoLogicMask_[16];
	static uint_fast16_t AlgoDwScMask_[16];
	static uint16_t AlgoDwScFactor_[16][10]; //change the second dimension with the number of implemented algos
	static int AlgoProcessID_[16][10];

	static uint_fast16_t ChodEnableMask_;
	static uint_fast16_t RichEnableMask_;
	static uint_fast16_t CedarEnableMask_;
	static uint_fast16_t LavEnableMask_;
	static uint_fast16_t StrawEnableMask_;
	static uint_fast16_t IrcSacEnableMask_;
	static uint_fast16_t MuvEnableMask_;
	static uint_fast16_t NewChodEnableMask_;

	static uint_fast16_t ChodFlagMask_;
	static uint_fast16_t RichFlagMask_;
	static uint_fast16_t CedarFlagMask_;
	static uint_fast16_t LavFlagMask_;
	static uint_fast16_t StrawFlagMask_;
	static uint_fast16_t IrcSacFlagMask_;
	static uint_fast16_t MuvFlagMask_;
	static uint_fast16_t NewChodFlagMask_;

	// Downscaling variables GLM:???
	static uint ChodAlgorithmId_;
	static uint RichAlgorithmId_;
	static uint CedarAlgorithmId_;
	static uint LavAlgorithmId_;
	static uint StrawAlgorithmId_;
	static uint IrcSacAlgorithmId_;
	static uint MuvAlgorithmId_;
	static uint NewChodAlgorithmId_;

	static int StrawAlgoType_[16];
	static int MUVAlgoType_[16];

	// Converters
	static uint MaskIDToNum_[16];
	static uint NumToMaskID_[16];
	static uint AlgoIDToNum_[16][10]; //change the second dimension with the number of implemented algos
	static uint NumToAlgoID_[16][10];

	static uint_fast8_t NumberOfEnabledL0Masks_;
	static std::vector<int> L0MaskIDs_;

	static uint_fast32_t L1DataPacketSize_;

}
;

} /* namespace na62 */
#endif /* TRIGGERPROCESSOR_H_ */
