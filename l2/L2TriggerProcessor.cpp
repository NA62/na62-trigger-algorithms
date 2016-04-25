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
std::atomic<uint64_t> L2TriggerProcessor::L2BypassedEvents_(0);
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

void L2TriggerProcessor::initialize(l2Struct &l2Struct) {

	for (int i = 0; i != 0xFF + 1; i++) {
		L2Triggers_[i] = 0;
	}
//	bypassProbability = TriggerOptions::GetDouble(OPTION_L2_BYPASS_PROBABILITY);
//	l2ReductionFactor = Options::GetInt(OPTION_L2_REDUCTION_FACTOR);
//	l2DownscaleFactor = Options::GetInt(OPTION_L2_DOWNSCALE_FACTOR);
//	l2FlagMode = Options::GetInt(OPTION_L2_FLAG_MODE);
//	l2AutoFlagFactor = Options::GetInt(OPTION_L2_AUTOFLAG_FACTOR);

	bypassProbability = l2Struct.l2Global.l2BypassProbability;
	reductionFactor = l2Struct.l2Global.l2ReductionFactor;
	downscaleFactor = l2Struct.l2Global.l2DownscaleFactor;
	flagMode = (bool) l2Struct.l2Global.l2FlagMode;
	autoFlagFactor = l2Struct.l2Global.l2AutoFlagFactor;
	referenceTimeSourceID = l2Struct.l2Global.l2ReferenceTimeSourceID;
/*
	LOG_INFO << "L2: byPassProb " << bypassProbability << " autoFlagFactor " << autoFlagFactor << " redFactor "
				<< reductionFactor << " dwscFactor " << downscaleFactor << " flagMode " << flagMode
				<< " refTimeID " << referenceTimeSourceID << ENDL;
*/
	/*
	 * Initialisation of individual Algo masks
	 */
//	for (int i = 0; i != 16; i++) {
//		LKrAlgo::initialize(l2Struct.l2Mask[i].lkr);
//	}

}

uint_fast8_t L2TriggerProcessor::compute(Event* event) {

	L2InputEvents_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO<< "L2Event number after adding 1 " << L2InputEvents_ << ENDL;
	L2InputEventsPerBurst_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO<< "L2Event number per Burst after adding 1 " << L2InputEventsPerBurst_ << ENDL;
	/*
	 * Check if the event should bypass the processing
	 */
	if (event->isSpecialTriggerEvent()) {
		return TRIGGER_L2_BYPASS;
	}
	if (event->isL2Bypassed() || bypassEvent()) {
		L2BypassedEvents_.fetch_add(1, std::memory_order_relaxed);
//		LOG_INFO<< "L2 ByPassed Event number after adding 1 " << L2BypassedEvents_ << ENDL;
		return TRIGGER_L2_BYPASS;
	}
	/*
	 * Check if the event fulfills the reduction option
	 *
	 */
//	LOG_INFO<< "L2Reduction Factor " << reductionFactor << ENDL;
//	LOG_INFO<< "Modulo " << L2InputEvents_ % reductionFactor << ENDL;
	if (L2InputEvents_ % reductionFactor != 0)
		return 0;

	L2InputReducedEvents_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO<< "L2ReducedEvent number after adding 1 " << L2InputReducedEvents_ << ENDL;

	/*
	 * The event is ready to be processed
	 *
	 */
	uint_fast8_t l2FlagTrigger = 0;
	if (flagMode || (L2InputReducedEvents_ % autoFlagFactor == 0)) {
		l2FlagTrigger = 1;
	} else {
		l2FlagTrigger = 0;
	}

	uint_fast8_t l2Trigger = 3;

	l2Trigger |= (l2FlagTrigger << 7);
//	printf("L2TriggerProcessor.cpp: l2Trigger %x\n", l2Trigger);

	L2Triggers_[l2Trigger].fetch_add(1, std::memory_order_relaxed);

	if (l2Trigger != 0) {
		L2AcceptedEvents_.fetch_add(1, std::memory_order_relaxed);
//		LOG_INFO<<"L2 Accepted Event number after adding 1 " << L2AcceptedEvents_ << ENDL;

//Global L2 downscaling
//		LOG_INFO<< "L2Downscale Factor " << downscaleFactor << ENDL;
//		LOG_INFO<< "Modulo " << L2AcceptedEvents_ % downscaleFactor << ENDL;
		if (L2AcceptedEvents_ % downscaleFactor != 0)
			return 0;
	}
	return l2Trigger;
}

uint_fast8_t L2TriggerProcessor::onNonZSuppressedLKrDataReceived(
		Event * event) {
	LOG_INFO
			<< "onNonZSuppressedLKrDataReceived - Trigger method not yet implemented!!!!!!"
			<< ENDL;
	return 1;
}

void L2TriggerProcessor::async_requestNonZSuppressedLKrData(
		const std::vector<uint_fast16_t> crateCREAMIDs, Event* event) {
//	event->setNonZSuppressedDataRequestedNum((uint_fast16_t) crateCREAMIDs.size());
//	cream::L1DistributionHandler::Async_RequestLKRDataUnicast(event,
//	true, crateCREAMIDs);
}

} /* namespace na62 */
