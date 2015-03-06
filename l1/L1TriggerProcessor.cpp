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

uint_fast8_t L1TriggerProcessor::bypassTriggerWord;
double L1TriggerProcessor::bypassProbability;
uint L1TriggerProcessor::cedarAlgorithmId;

void L1TriggerProcessor::registerDownscalingAlgorithms() {
	cedarAlgorithmId = L1Downscaling::registerAlgorithm("CEDAR");
}

void L1TriggerProcessor::initialize(double _bypassProbability,
		uint _bypassTriggerWord) {
	// Seed for rand()
	srand(time(NULL));

	bypassProbability = _bypassProbability;
	bypassTriggerWord = _bypassTriggerWord;

	L1Downscaling::initialize();
}

uint8_t L1TriggerProcessor::compute(Event* event) {
	using namespace l0;

	/*
	 * Check if the event should bypass the processing
	 */
	if (bypassEvent() || event->isSpecialTriggerEvent()) {
		return bypassTriggerWord;
	}

	uint8_t cedarTrigger = 0;

//	cedarTrigger = KtagAlgo::processKtagTrigger(event);

	if (L1Downscaling::processAlgorithm(cedarAlgorithmId)) {
		cedarTrigger = KtagAlgo::processKtagTrigger(event);
		if (!cedarTrigger) {
			return 0;
		}
	} else {
		return 0;
	}

	//event->setProcessingID(0); // 0 indicates raw data as collected from the detector
	return cedarTrigger;
}

} /* namespace na62 */
