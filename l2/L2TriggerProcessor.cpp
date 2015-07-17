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
#include <eventBuilding/L2Builder.h>
#include <iostream>
#include "L2Fragment.h"

namespace na62 {

double L2TriggerProcessor::bypassProbability;

void L2TriggerProcessor::initialize(double _bypassProbability) {
	bypassProbability = _bypassProbability;
}

uint_fast8_t L2TriggerProcessor::compute(Event* event) {
	using namespace cream;

	const l0::MEPFragment* const L2Fragment =
			event->getL2Subevent()->getFragment(0);

	const char* payload = L2Fragment->getPayload();
	L2_BLOCK * l2Block = (L2_BLOCK *) (payload);

	// Setting the new globalDownscaleFactor and globalReductionFactor in L2Block

//	uint globDownFactor = L2Builder::GetL2DownscaleFactor();
//	l2Block->globaldownscaling = globDownFactor;

//	uint globReducFactor = L2Builder::GetL2ReductionFactor();
//	l2Block->globalreduction = globReducFactor;

	/*
	 * Check if the event should bypass the processing
	 */
	if (event->isL2Bypassed() || bypassEvent()
			|| event->isSpecialTriggerEvent()) {
		l2Block->triggerword = TRIGGER_L2_BYPASS;
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
	uint_fast8_t l2Trigger = 3;
	l2Block->triggerword = l2Trigger;

// Accept event
	return l2Trigger;
}

uint_fast8_t L2TriggerProcessor::onNonZSuppressedLKrDataReceived(Event* event) {
	LOG_INFO<< "onNonZSuppressedLKrDataReceived - Trigger method not yet implemented!!!!!!!!!!!!"
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
