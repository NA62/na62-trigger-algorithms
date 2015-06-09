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

#include "../common/decoding/DecoderHandler.h"
#include "L1Downscaling.h"
#include "KtagAlgo.h"
#include "CHODAlgo.h"
#include "RICHAlgo.h"

namespace na62 {

double L1TriggerProcessor::bypassProbability;
uint L1TriggerProcessor::cedarAlgorithmId;
uint L1TriggerProcessor::chodAlgorithmId;
uint L1TriggerProcessor::richAlgorithmId;

void L1TriggerProcessor::registerDownscalingAlgorithms() {
	cedarAlgorithmId = L1Downscaling::registerAlgorithm("CEDAR");
	chodAlgorithmId = L1Downscaling::registerAlgorithm("CHOD");
	richAlgorithmId = L1Downscaling::registerAlgorithm("RICH");
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
	return l1TriggerTypeWord != TRIGGER_L1_BYPASS;
}

uint_fast8_t L1TriggerProcessor::compute(Event* const event) {
	using namespace l0;
	DecoderHandler decoder(event);

	event->readTriggerTypeWordAndFineTime();

	/*
	 * Check if the event should bypass the processing
	 */
	if (bypassEvent() || event->isSpecialTriggerEvent()) {
		// Request zero suppressed CREAM data for bypassed events?
		event->setRrequestZeroSuppressedCreamData(
				isRequestZeroSuppressedCreamData(TRIGGER_L1_BYPASS));

		return TRIGGER_L1_BYPASS;
	}

	uint_fast8_t l1Trigger = 1;
//	uint_fast8_t l1Trigger = 0;
	uint_fast8_t cedarTrigger = 0;
	uint_fast8_t chodTrigger = 0;
	uint_fast8_t richTrigger = 0;

	if (L1Downscaling::processAlgorithm(cedarAlgorithmId)) {
		if (SourceIDManager::isCedarActive()) {
			cedarTrigger = KtagAlgo::processKtagTrigger(decoder);
		}
	} else {
		cedarTrigger = 0xFF;
	}
	//	printf("L1TriggerProcessor.cpp: cedarTrigger %d\n",cedarTrigger);

	if (L1Downscaling::processAlgorithm(chodAlgorithmId)) {
		if (SourceIDManager::isChodActive()) {
			chodTrigger = CHODAlgo::processCHODTrigger(decoder);
		}
	} else {
		chodTrigger = 0xFF;
	}
	//	printf("L1TriggerProcessor.cpp: chodTrigger %d\n",chodTrigger);

	if (L1Downscaling::processAlgorithm(richAlgorithmId)) {
		if (SourceIDManager::isRhichActive()) {
			richTrigger = RICHAlgo::processRICHTrigger(decoder);
		}
	} else {
		richTrigger = 0xFF;
	}
	//	printf("L1TriggerProcessor.cpp: richTrigger %d\n",richTrigger);

	/*
	 * Final L1 trigger word calculation
	 */
//	l1Trigger = cedarTrigger;
//	l1Trigger = chodTrigger;
//	l1Trigger = richTrigger;
	l1Trigger = (l1Trigger << 7) | (cedarTrigger << 2) | (richTrigger << 1)
			| chodTrigger;
//	printf("L1TriggerProcessor.cpp: l1Trigger %d\n",l1Trigger);
	/*
	 * Decision whether or not to request zero suppressed data from the creams
	 */
	event->setRrequestZeroSuppressedCreamData(
			isRequestZeroSuppressedCreamData(l1Trigger));
	event->setProcessingID(0); // 0 indicates raw data as collected from the detector
	return l1Trigger;
}

} /* namespace na62 */
