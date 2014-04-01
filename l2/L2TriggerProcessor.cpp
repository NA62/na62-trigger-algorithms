/*
 * L2TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunzej@cern.ch)
 */

#include "L2TriggerProcessor.h"

#include <iostream>

#include <eventBuilding/Event.h>
#include <LKr/L1DistributionHandler.h>
#include <options/Options.h>

namespace na62 {

int L2_DOWNSCALE_FACTOR = 0;

L2TriggerProcessor::L2TriggerProcessor(const uint16_t ThreadNum) :
		ThreadNum_(ThreadNum), rr(0) {

	L2_DOWNSCALE_FACTOR = Options::GetInt(OPTION_L2_DOWNSCALE_FACTOR);
}

L2TriggerProcessor::~L2TriggerProcessor() {
}

uint8_t L2TriggerProcessor::compute(Event* event) {
	using namespace cream;

//	std::vector<uint16_t> localCreamIDsToRequestNonZSuppressedData;
//
//	if (Options::Instance()->TEST > 0) {
//		uint32_t sum = 0;
//		for (int localCreamID = event->getNumberOfZSuppressedLKrEvents() - 1; localCreamID != -1; localCreamID--) {
//			LKREvent* lkrEvent = event->getZSuppressedLKrEvent(localCreamID);
//			localCreamIDsToRequestNonZSuppressedData.push_back(lkrEvent->getCrateCREAMID());
//			const char* data = lkrEvent->getDataWithHeader();
//			for (int j = lkrEvent->getEventLength() - 1; j >= 0; j--) {
//				sum += data[j];
//			}
//		}
//	}

//	async_requestNonZSuppressedLKrData(localCreamIDsToRequestNonZSuppressedData, event);

	if (rr++ % L2_DOWNSCALE_FACTOR == 0) {
		// Accept event
		return 3;
	}
	return 0;
}

uint8_t L2TriggerProcessor::onNonZSuppressedLKrDataReceived(Event* event) {
	std::cout << "onNonZSuppressedLKrDataReceived!!!!!!!!!!!!" << std::endl;
	return 1;
}

void L2TriggerProcessor::async_requestNonZSuppressedLKrData(
		const std::vector<uint16_t> crateCREAMIDs, Event* event) {
	event->setNonZSuppressedDataRequestedNum((uint16_t) crateCREAMIDs.size());
	cream::L1DistributionHandler::Async_RequestLKRDataUnicast(ThreadNum_, event,
	true, crateCREAMIDs);
}
} /* namespace na62 */
