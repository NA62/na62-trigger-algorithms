/*
 * L1TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L1TriggerProcessor.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>

namespace na62 {

uint8_t L1TriggerProcessor::compute(Event* event) {
	using namespace l0;

	event->setProcessingID(0); // 0 indicates raw data as collected from the detector

	return 1;
}
} /* namespace na62 */
