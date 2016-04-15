/*
 * L2TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L2TriggerProcessor.h"

#include <options/Logging.h>
#include <eventBuilding/Event.h>
#include <l0/Subevent.h>
#include <iostream>
#include "L2Fragment.h"

namespace na62 {

double L2TriggerProcessor::bypassProbability;

void L2TriggerProcessor::initialize(double _bypassProbability) {
	bypassProbability = _bypassProbability;
}

uint_fast8_t L2TriggerProcessor::compute(Event* event) {

	/*
	 * Check if the event should bypass the processing
	 */
	if (event->isL2Bypassed() || bypassEvent()
			|| event->isSpecialTriggerEvent()) {
//		l2Block->triggerword = TRIGGER_L2_BYPASS;
		return TRIGGER_L2_BYPASS;
	}

	uint_fast8_t l2Trigger = 3;

	return l2Trigger;
}

uint_fast8_t L2TriggerProcessor::onNonZSuppressedLKrDataReceived(Event* event) {
	LOG_INFO("onNonZSuppressedLKrDataReceived - Trigger method not yet implemented!!!!!!!!!!!!");
	return 1;
}

void L2TriggerProcessor::async_requestNonZSuppressedLKrData(
		const std::vector<uint_fast16_t> crateCREAMIDs, Event* event) {
//	event->setNonZSuppressedDataRequestedNum((uint_fast16_t) crateCREAMIDs.size());
//	cream::L1DistributionHandler::Async_RequestLKRDataUnicast(event,
//	true, crateCREAMIDs);
}

} /* namespace na62 */
