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

#include "../common/headers/L0TPHeader.h"

namespace na62 {

uint16_t L1TriggerProcessor::compute(Event* event) {
	using namespace l0;

	/*
	 * Store
	 */
	l0::MEPFragment* L0TPEvent = event->getL0TPSubevent()->getFragment(0);
	L0TpHeader* L0TPData = (L0TpHeader*) L0TPEvent->getPayload();
	event->setFinetime(L0TPData->refFineTime);

	event->setProcessingID(0); // 0 indicates raw data as collected from the detector

	uint8_t l0TriggerTypeWord = L0TPData->l0TriggerType;
	uint8_t l1TriggerTypeWord = 1; // Accept event

	return (l0TriggerTypeWord | l1TriggerTypeWord << 8);
}
} /* namespace na62 */
