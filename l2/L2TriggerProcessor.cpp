/*
 * L2TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L2TriggerProcessor.h"

#include <options/Logging.h>
#include "../options/TriggerOptions.h"
#include <l0/Subevent.h>
#include <iostream>
#include "L2Fragment.h"

namespace na62 {

std::atomic<uint64_t>* L2TriggerProcessor::L2Triggers_ = new std::atomic<uint64_t>[0xFF + 1];
std::atomic<uint64_t> L2TriggerProcessor::L2InputEvents_(0);
std::atomic<uint64_t> L2TriggerProcessor::L2InputReducedEvents_(0);
std::atomic<uint64_t> L2TriggerProcessor::L2InputEventsPerBurst_(0);
std::atomic<uint64_t> L2TriggerProcessor::L2AcceptedEvents_(0);

double L2TriggerProcessor::BypassProbability_;
uint L2TriggerProcessor::ReductionFactor_ = 0;
uint L2TriggerProcessor::DownscaleFactor_ = 0;
uint L2TriggerProcessor::FlagMode_ = 0;
uint L2TriggerProcessor::AutoFlagFactor_ = 0;
uint L2TriggerProcessor::ReferenceTimeSourceID_ = 0;

uint L2TriggerProcessor::NumberOfEnabledAlgos_[16];
uint L2TriggerProcessor::NumberOfFlaggedAlgos_[16];
uint L2TriggerProcessor::MaskReductionFactor_[16];
uint_fast16_t L2TriggerProcessor::AlgoEnableMask_[16];
uint_fast16_t L2TriggerProcessor::AlgoFlagMask_[16];
uint_fast16_t L2TriggerProcessor::AlgoLogicMask_[16];
uint_fast16_t L2TriggerProcessor::AlgoDwScMask_[16];

uint L2TriggerProcessor::MaskIDToNum_[16];
uint L2TriggerProcessor::NumToMaskID_[16];

uint_fast8_t L2TriggerProcessor::NumberOfEnabledL0Masks_ = 0;

std::vector<int> L2TriggerProcessor::L0MaskIDs_;

uint_fast32_t L2TriggerProcessor::L2DataPacketSize_ = 0;

void L2TriggerProcessor::initialize(l2Struct &l2Struct) {

	L0MaskIDs_ = TriggerOptions::GetIntList(OPTION_ACTIVE_L0_MASKS);
	NumberOfEnabledL0Masks_ = L0MaskIDs_.size();

	for (int i = 0; i != 0xFF + 1; i++) {
		L2Triggers_[i] = 0;
	}

	/*
	 * Initialisation of individual Algo masks
	 */
	for (int i = 0; i != 16; i++) {
//		LKrAlgo::initialize(l2Struct.l2Mask[i].lkr);
		NumToMaskID_[i] = -1;
		MaskIDToNum_[i] = -1;

		NumberOfEnabledAlgos_[i] = 0; //TO BE MODIFIED WITH REAL NUMBER !!!!!!!!
		NumberOfFlaggedAlgos_[i] = 0;
		MaskReductionFactor_[i] = 0;
		AlgoEnableMask_[i] = 0;
		AlgoFlagMask_[i] = 0;
		AlgoLogicMask_[i] = 0;
		AlgoDwScMask_[i] = 0;
	}

	BypassProbability_ = l2Struct.l2Global.l2BypassProbability;
	ReductionFactor_ = l2Struct.l2Global.l2ReductionFactor;
	DownscaleFactor_ = l2Struct.l2Global.l2DownscaleFactor;
	FlagMode_ = (bool) l2Struct.l2Global.l2FlagMode;
	AutoFlagFactor_ = l2Struct.l2Global.l2AutoFlagFactor;
	ReferenceTimeSourceID_ = l2Struct.l2Global.l2ReferenceTimeSourceID;

	L2DataPacketSize_ = sizeof(L2_BLOCK);

}

uint_fast8_t L2TriggerProcessor::compute(Event* event) {

	bool isL0PhysicsTrigger = false;
	bool isL0PeriodicTrigger = false;
	bool isL0ControlTrigger = false;
	bool isL2Bypassed = false;
	uint numberOfTriggeredL2Masks = 0;
	bool isAllL2AlgoDisable = false;

	uint_fast8_t l2Trigger = 0;
	uint_fast8_t l2GlobalFlagTrigger = 0;
	uint_fast8_t l2MaskFlagTrigger = 0;

	L2InputEvents_.fetch_add(1, std::memory_order_relaxed);
	L2InputEventsPerBurst_.fetch_add(1, std::memory_order_relaxed);

	if (FlagMode_) {
		l2GlobalFlagTrigger = 1;
	} else if (AutoFlagFactor_ && (L2InputEvents_ % AutoFlagFactor_ == 0)) {
		l2GlobalFlagTrigger = 1;
	}

	/*
	 * Check if the event should bypass the processing
	 */
	if (event->isSpecialTriggerEvent()) {
		isL2Bypassed = true;
		l2Trigger = ((uint) l2GlobalFlagTrigger << 7) | ((l2MaskFlagTrigger != 0) << 6) | (isL2Bypassed << 5) | (isAllL2AlgoDisable << 4)
				| (numberOfTriggeredL2Masks != 0);
		L2Triggers_[l2Trigger].fetch_add(1, std::memory_order_relaxed);
		writeData(event, l2Trigger);
		return l2Trigger;
	}
	if (event->isPulserGTKTriggerEvent() || event->isPeriodicTriggerEvent() || event->isL2Bypassed() || bypassEvent()) {
		isL2Bypassed = true;
		l2Trigger = ((uint) l2GlobalFlagTrigger << 7) | ((l2MaskFlagTrigger != 0) << 6) | (isL2Bypassed << 5) | (isAllL2AlgoDisable << 4)
				| (numberOfTriggeredL2Masks != 0);
		L2Triggers_[l2Trigger].fetch_add(1, std::memory_order_relaxed);
		writeData(event, l2Trigger);
		return l2Trigger;
	}

	/*
	 * Check if the event fulfills the reduction option
	 *
	 */
	if (ReductionFactor_ && (L2InputEvents_ % ReductionFactor_ != 0))
		return 0;

	L2InputReducedEvents_.fetch_add(1, std::memory_order_relaxed);

	/*
	 * The event is ready to be processed
	 *
	 */
	uint_fast16_t l0TrigFlags = event->getTriggerFlags();
	uint_fast8_t l2TriggerResult;
	uint_fast8_t l2TriggerTmp;

	if (event->isPhysicsTriggerEvent()) {
		isL0PhysicsTrigger = 1;
	}
	if (event->isPeriodicTriggerEvent()) {
		isL0PeriodicTrigger = 1;
		isL2Bypassed = 1;
	}
	if (event->isControlTriggerEvent()) {
		isL0ControlTrigger = 1;
		isL2Bypassed = 1;
	}

	if (isL0PhysicsTrigger) {
		for (int i = 0; i != 16; i++) {
			if (l0TrigFlags & (1 << i)) {
				l2TriggerResult = 0;
				l2TriggerTmp = 0;
				if (!NumberOfEnabledAlgos_[i])
					isAllL2AlgoDisable = 1;

				l2TriggerResult = ((l2TriggerTmp & AlgoEnableMask_[i]) == AlgoEnableMask_[i]);
			}
			if (__builtin_popcount((uint) l2TriggerResult))
				numberOfTriggeredL2Masks++;

			event->setL2TriggerWord(i, l2TriggerResult);
		}
	}

	l2Trigger = ((uint) l2GlobalFlagTrigger << 7) | ((l2MaskFlagTrigger != 0) << 6) | (isL2Bypassed << 5) | (isAllL2AlgoDisable << 4)
			| (numberOfTriggeredL2Masks != 0);

	if (l2Trigger != 0) {
		L2AcceptedEvents_.fetch_add(1, std::memory_order_relaxed);

//Global L2 downscaling
		if (DownscaleFactor_ && (L2AcceptedEvents_ % DownscaleFactor_ != 0))
			return 0;
	}

	L2Triggers_[l2Trigger].fetch_add(1, std::memory_order_relaxed);
	writeData(event, l2Trigger);
	return l2Trigger;
}

uint_fast8_t L2TriggerProcessor::onNonZSuppressedLKrDataReceived(Event* event) {
	LOG_INFO("onNonZSuppressedLKrDataReceived - Trigger method not yet implemented!!!!!!!!!!!!");
	return 1;
}

void L2TriggerProcessor::async_requestNonZSuppressedLKrData(const std::vector<uint_fast16_t> crateCREAMIDs, Event* event) {
//	event->setNonZSuppressedDataRequestedNum((uint_fast16_t) crateCREAMIDs.size());
//	cream::L1DistributionHandler::Async_RequestLKRDataUnicast(event,
//	true, crateCREAMIDs);
}

void L2TriggerProcessor::writeData(Event* event, const uint32_t& l2TriggerWord) {

	const l0::MEPFragment* const L2TPEvent = event->getL2ResultSubevent()->getFragment(0);

	if (L2TPEvent->getPayloadLength() != L2DataPacketSize_) {
		LOG_ERROR(
				"L2 result fragment has wrong size " << (int) L2TPEvent->getPayloadLength()<< " instead of " << L2DataPacketSize_ << ". Result will not be filled.");
		return;
	}

	char* payload = const_cast<char*>(L2TPEvent->getPayload());
	// This will eventually become a more complex result...
	L2_BLOCK* globalResult = reinterpret_cast<L2_BLOCK*>(payload);
	globalResult->triggerword = l2TriggerWord;

	/* No L2 Algorithms implemented yet - result is very simple
	 (l2Block.l2Global).l2BypassProbability = BypassProbability_;
	 (l2Block.l2Global).l2FlagMode = FlagMode_;
	 (l2Block.l2Global).refTimeSourceID = ReferenceTimeSourceID_;
	 (l2Block.l2Global).l2AutoFlagFactor = AutoFlagFactor_;
	 (l2Block.l2Global).l2ReductionFactor = ReductionFactor_;
	 (l2Block.l2Global).l2DownscaleFactor = DownscaleFactor_;

	 int numToMaskID;
	 for (int iNum = 0; iNum < NumberOfEnabledL0Masks_; iNum++) {
	 if (NumToMaskID_[iNum] == -1)
	 LOG_ERROR("ERROR! Wrong association of mask ID!");
	 else
	 numToMaskID = NumToMaskID_[iNum];
	 (l2Block.l2Mask[iNum]).maskID = numToMaskID;
	 (l2Block.l2Mask[iNum]).triggerWord = l2TriggerWords[numToMaskID];
	 (l2Block.l2Mask[iNum]).numberOfEnabledAlgos =
	 NumberOfEnabledAlgos_[numToMaskID];
	 (l2Block.l2Mask[iNum]).numberOfFlaggedAlgos =
	 NumberOfFlaggedAlgos_[numToMaskID];


	 (l2Block.l2Mask[iNum]).reductionFactor =
	 MaskReductionFactor_[numToMaskID];
	 (l2Block.l2Mask[iNum]).algoEnableMask = AlgoEnableMask_[numToMaskID];
	 (l2Block.l2Mask[iNum]).algoFlagMask = AlgoFlagMask_[numToMaskID];
	 (l2Block.l2Mask[iNum]).algoLogicMask = AlgoLogicMask_[numToMaskID];
	 (l2Block.l2Mask[iNum]).algoDwScMask = AlgoDwScMask_[numToMaskID];

	 }
	 */
}

} /* namespace na62 */
