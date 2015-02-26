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

#include "KtagAlgo.h"

namespace na62 {

uint_fast8_t L1TriggerProcessor::bypassTriggerWord;
double L1TriggerProcessor::bypassProbability;

void L1TriggerProcessor::initialize(double _bypassProbability,
		uint _bypassTriggerWord) {
	// Seed for rand()
	srand (time(NULL));

	bypassProbability = _bypassProbability;
	bypassTriggerWord = _bypassTriggerWord;
}

uint8_t L1TriggerProcessor::compute(Event* event) {
	using namespace l0;

	/*
	 * Check if the event should bypass the processing
	 */
	if (bypassEvent()) {
		return bypassTriggerWord;
	}

	uint8_t trigger = KtagAlgo::checkKtagTrigger(event);
	if (trigger) {
//		LOG_INFO << "event number = " << event->getEventNumber() << ENDL;
//		LOG_INFO << "GOOD EVENT! " << ENDL;
		return trigger;
	} else
//		LOG_INFO << "BAD EVENT! " << ENDL;

	//event->setProcessingID(0); // 0 indicates raw data as collected from the detector
	return 0;

}

} /* namespace na62 */
