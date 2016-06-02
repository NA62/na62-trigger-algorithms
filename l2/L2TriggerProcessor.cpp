/*
 * L2TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L2TriggerProcessor.h"

#include <options/Logging.h>
#include "../options/TriggerOptions.h"
#include <eventBuilding/Event.h>
#include <l0/Subevent.h>
#include <iostream>
#include "L2Fragment.h"

namespace na62 {

std::atomic<uint64_t>* L2TriggerProcessor::L2Triggers_ = new std::atomic<
		uint64_t>[0xFF + 1];
std::atomic<uint64_t> L2TriggerProcessor::L2InputEvents_(0);
std::atomic<uint64_t> L2TriggerProcessor::L2InputReducedEvents_(0);
std::atomic<uint64_t> L2TriggerProcessor::L2InputEventsPerBurst_(0);
std::atomic<uint64_t> L2TriggerProcessor::L2AcceptedEvents_(0);

double L2TriggerProcessor::bypassProbability;
uint L2TriggerProcessor::reductionFactor = 0;
uint L2TriggerProcessor::downscaleFactor = 0;
uint L2TriggerProcessor::flagMode = 0;
uint L2TriggerProcessor::autoFlagFactor = 0;
uint L2TriggerProcessor::referenceTimeSourceID = 0;

uint L2TriggerProcessor::numberOfEnabledAlgos[16];

uint_fast8_t L2TriggerProcessor::l0TrigWord = 0;
uint_fast8_t L2TriggerProcessor::l0DataType = 0;
uint_fast16_t L2TriggerProcessor::l0TrigFlags = 0;

uint_fast8_t L2TriggerProcessor::l2TriggerWords[16];

bool L2TriggerProcessor::isL0PhysicsTrigger = 0;
bool L2TriggerProcessor::isL0PeriodicTrigger = 0;
bool L2TriggerProcessor::isL0ControlTrigger = 0;
bool L2TriggerProcessor::isL2Bypassed = 0;
uint L2TriggerProcessor::numberOfTriggeredL2Masks = 0;
bool L2TriggerProcessor::isAllL2AlgoDisable = 0;

void L2TriggerProcessor::initialize(l2Struct &l2Struct) {

	for (int i = 0; i != 0xFF + 1; i++) {
		L2Triggers_[i] = 0;
	}
	for (int i = 0; i != 16; i++) {
		l2TriggerWords[i] = 0;
		numberOfEnabledAlgos[i] = 0; //TO BE MODIFIED WITH REAL NUMBER !!!!!!!!
	}

	bypassProbability = l2Struct.l2Global.l2BypassProbability;
	reductionFactor = l2Struct.l2Global.l2ReductionFactor;
	downscaleFactor = l2Struct.l2Global.l2DownscaleFactor;
	flagMode = (bool) l2Struct.l2Global.l2FlagMode;
	autoFlagFactor = l2Struct.l2Global.l2AutoFlagFactor;
	referenceTimeSourceID = l2Struct.l2Global.l2ReferenceTimeSourceID;

	/*
	 * Initialisation of individual Algo masks
	 */
//	for (int i = 0; i != 16; i++) {
//		LKrAlgo::initialize(l2Struct.l2Mask[i].lkr);
//	}
}

uint_fast8_t L2TriggerProcessor::compute(Event* event) {

	event->readTriggerTypeWordAndFineTime();

	uint_fast8_t l2Trigger = 0;
	uint_fast8_t l2GlobalFlagTrigger = 0;
	uint_fast8_t l2MaskFlagTrigger = 0;
	isL2Bypassed = 0;
	isAllL2AlgoDisable = 0;
	numberOfTriggeredL2Masks = 0;

	L2InputEvents_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO("-------------- L2Event number after adding 1 " << L2InputEvents_);
	L2InputEventsPerBurst_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO("--------------- L2Event number per Burst after adding 1 " << L2InputEventsPerBurst_);

//	LOG_INFO("Global FlagMode " << flagMode << " " << L2InputEvents_ << " " << autoFlagFactor);

	if (flagMode) {
		l2GlobalFlagTrigger = 1;
	} else if (autoFlagFactor && (L2InputEvents_ % autoFlagFactor == 0)) {
		l2GlobalFlagTrigger = 1;
	}
//	LOG_INFO("l2GlobalFlagTrigger " << (uint)l2GlobalFlagTrigger);

	/*
	 * Check if the event should bypass the processing
	 */
	if (event->isSpecialTriggerEvent()) {
		isL2Bypassed = 1;
		l2Trigger = ((uint) l2GlobalFlagTrigger << 7)
				| ((l2MaskFlagTrigger != 0) << 6) | (isL2Bypassed << 5)
				| (isAllL2AlgoDisable << 4) | (numberOfTriggeredL2Masks != 0);
		L2Triggers_[l2Trigger].fetch_add(1, std::memory_order_relaxed);
		return l2Trigger;
	}
	if (event->isPulserGTKTriggerEvent() || event->isL2Bypassed()
			|| bypassEvent()) {
		isL2Bypassed = 1;
		l2Trigger = ((uint) l2GlobalFlagTrigger << 7)
				| ((l2MaskFlagTrigger != 0) << 6) | (isL2Bypassed << 5)
				| (isAllL2AlgoDisable << 4) | (numberOfTriggeredL2Masks != 0);
		L2Triggers_[l2Trigger].fetch_add(1, std::memory_order_relaxed);
		return l2Trigger;
	}

	/*
	 * Check if the event fulfills the reduction option
	 *
	 */
//	LOG_INFO("L2Reduction Factor " << reductionFactor);
//	LOG_INFO("Modulo " << L2InputEvents_ % reductionFactor);
	if (reductionFactor && (L2InputEvents_ % reductionFactor != 0))
		return 0;

	L2InputReducedEvents_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO("L2ReducedEvent number after adding 1 " << L2InputReducedEvents_);

	/*
	 * The event is ready to be processed
	 *
	 */
	l0TrigWord = event->getL0TriggerTypeWord();
//	LOG_INFO("l0TriggerWord " << std::hex << (uint) l0TrigWord << std::dec);
	l0TrigFlags = event->getTriggerFlags();
//	LOG_INFO("l0TriggerFlags " << std::hex << (uint) l0TrigFlags << std::dec);
	l0DataType = event->getTriggerDataType();
//	LOG_INFO("l0TriggerDataType " << std::hex << (uint) l0DataType << std::dec);

	isL0PhysicsTrigger = 0;
	isL0PeriodicTrigger = 0;
	isL0ControlTrigger = 0;

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
			l2TriggerWords[i] = 0;
			if (l0TrigFlags & (1 << i)) {
				if (!numberOfEnabledAlgos[i])
					isAllL2AlgoDisable = 1;
//				LOG_INFO("i " << i << " nEnabledAlgos " << numberOfEnabledAlgos[i] << " isAllAlgoDisable " << isAllL2AlgoDisable);
			}
			if (__builtin_popcount((uint) l2TriggerWords[i]))
				numberOfTriggeredL2Masks++;
		}
//		printf("Summary of Triggered Masks: %d\n", numberOfTriggeredL2Masks);
//		for (int i = 0; i != 16; i++) printf("Summary of Trigger Words: l2Trigger %x\n",l2TriggerWords[i]);
	}

	l2Trigger = ((uint) l2GlobalFlagTrigger << 7)
			| ((l2MaskFlagTrigger != 0) << 6) | (isL2Bypassed << 5)
			| (isAllL2AlgoDisable << 4) | (numberOfTriggeredL2Masks != 0);

	if (l2Trigger != 0) {
		L2AcceptedEvents_.fetch_add(1, std::memory_order_relaxed);
//		LOG_INFO("L2 Accepted Event number after adding 1 " << L2AcceptedEvents_);

//Global L2 downscaling
//		LOG_INFO("L2Downscale Factor " << downscaleFactor);
//		LOG_INFO("Modulo " << L2AcceptedEvents_ % downscaleFactor);
		if (downscaleFactor && (L2AcceptedEvents_ % downscaleFactor != 0))
			return 0;
	}

//	printf("L2TriggerProcessor.cpp: !!!!!!!! Final l2Trigger %x\n", l2Trigger);
	L2Triggers_[l2Trigger].fetch_add(1, std::memory_order_relaxed);
	return l2Trigger;
}

uint_fast8_t L2TriggerProcessor::onNonZSuppressedLKrDataReceived(Event* event) {
	LOG_INFO(
			"onNonZSuppressedLKrDataReceived - Trigger method not yet implemented!!!!!!!!!!!!");
	return 1;
}

void L2TriggerProcessor::async_requestNonZSuppressedLKrData(
		const std::vector<uint_fast16_t> crateCREAMIDs, Event* event) {
//	event->setNonZSuppressedDataRequestedNum((uint_fast16_t) crateCREAMIDs.size());
//	cream::L1DistributionHandler::Async_RequestLKRDataUnicast(event,
//	true, crateCREAMIDs);
}

} /* namespace na62 */
