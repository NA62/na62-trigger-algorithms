/*
 * L2TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L2TriggerProcessor.h"

#include <options/Logging.h>
#include <eventBuilding/Event.h>
#include <iostream>

namespace na62 {

double L2TriggerProcessor::bypassProbability;

void L2TriggerProcessor::initialize(double _bypassProbability) {
	bypassProbability = _bypassProbability;
}

uint_fast8_t L2TriggerProcessor::compute(Event* event) {
	using namespace cream;

	/*
	 * Check if the event should bypass the processing
	 */
	if (event->isL1Bypassed() || bypassEvent()) {
		return TRIGGER_L2_BYPASS;
	}

//	std::vector<uint_fast16_t> localCreamIDsToRequestNonZSuppressedData;
//	for (int localCreamID = event->getNumberOfZSuppressedLkrFragments() - 1;
//			localCreamID != -1; localCreamID--) {
//		LkrFragment* fragment = event->getZSuppressedLkrFragment(localCreamID);
//		localCreamIDsToRequestNonZSuppressedData.push_back(
//				fragment->getCrateCREAMID());
//		const char* data = fragment->getDataWithHeader();
//		const uint dataSize = fragment->getEventLength();
//	}

//	async_requestNonZSuppressedLKrData(localCreamIDsToRequestNonZSuppressedData, event);

// Accept event
	return 3;
}

uint_fast8_t L2TriggerProcessor::onNonZSuppressedLKrDataReceived(Event* event) {
	LOG_INFO
			<< "onNonZSuppressedLKrDataReceived - Trigger method not yet implemented!!!!!!!!!!!!"
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
