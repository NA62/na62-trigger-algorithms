/*
 * L1TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L1TriggerProcessor.h"
#include <options/Logging.h>

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>

#include "L1Downscaling.h"
#include "KtagAlgo.h"
#include "MultiDetAlgo.h"

namespace na62 {

double L1TriggerProcessor::bypassProbability;
uint L1TriggerProcessor::cedarAlgorithmId;

void L1TriggerProcessor::registerDownscalingAlgorithms() {
	cedarAlgorithmId = L1Downscaling::registerAlgorithm("CEDAR");
}

void L1TriggerProcessor::initialize(double _bypassProbability) {
	// Seed for rand()
	srand(time(NULL));

	bypassProbability = _bypassProbability;

	L1Downscaling::initialize();
}

bool L1TriggerProcessor::isRequestZeroSuppressedCreamData(
		uint_fast8_t l1TriggerTypeWord) {
	// add any special trigger here
	return l1TriggerTypeWord == TRIGGER_L1_BYPASS;
}

uint_fast8_t L1TriggerProcessor::compute(Event* event) {
	using namespace l0;

	/*
	 * Check if the event should bypass the processing
	 */
	if (bypassEvent() || event->isSpecialTriggerEvent()) {
		// Request zero suppressed CREAM data for bypassed events?
		event->setRrequestZeroSuppressedCreamData(isRequestZeroSuppressedCreamData(TRIGGER_L1_BYPASS));

		return TRIGGER_L1_BYPASS;
	}

	uint_fast8_t l1Trigger = 0;

	uint_fast8_t cedarTrigger = 0;
	if (L1Downscaling::processAlgorithm(cedarAlgorithmId)) {
		if (SourceIDManager::isCedarActive()) {
			cedarTrigger = KtagAlgo::processKtagTrigger(event);
		}
	} else {
		cedarTrigger = 0xFF;
	}

	/*
	 * Final L1 trigger word calculation
	 */
	l1Trigger = cedarTrigger;

	/*
	 * Decision whether or not to request zero suppressed data from the creams
	 */
	event->setRrequestZeroSuppressedCreamData(isRequestZeroSuppressedCreamData(l1Trigger));
	event->setProcessingID(0); // 0 indicates raw data as collected from the detector
	return l1Trigger;
}

} /* namespace na62 */
