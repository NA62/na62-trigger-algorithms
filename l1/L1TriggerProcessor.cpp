/*
 * L1TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L1TriggerProcessor.h"

#include <eventBuilding/Event.h>
#include <l0/MEPEvent.h>
#include <l0/Subevent.h>

namespace na62 {

uint L1TriggerProcessor::L1_DOWNSCALE_FACTOR = 0;

L1TriggerProcessor::L1TriggerProcessor() {
	rr = 0;
}

L1TriggerProcessor::~L1TriggerProcessor() {
}

int counter;
uint16_t L1TriggerProcessor::compute(Event* event) {
	using namespace l0;

//	// Access a specific detector:
//	Subevent* muv = event->getMUVSubevent();
//	for (int p = muv->getNumberOfParts() - 1; p >= 0; p--) {
//		MEPEvent* mepEvent = muv->getPart(p);
//		const MEPEVENT_HDR* data = mepEvent->getData();
//		mepEvent->getSourceID();
//		mepEvent->getData();
//		const uint dataSize = mepEvent->getDataLength();
//	}
//
//	// Access all detectors:
//	for (int i = SourceIDManager::NUMBER_OF_L0_DATA_SOURCES - 1; i >= 0; i--) {
//		Subevent* subevent = event->getL0SubeventBySourceIDNum(i);
//
//		for (int j = subevent->getNumberOfParts() - 1; j >= 0; j--) {
//			MEPEvent* mepEvent = subevent->getPart(j);
//
//			const MEPEVENT_HDR* data = mepEvent->getData();
//			const uint dataSize = mepEvent->getDataLength();
//		}
//	}

	/*
	 * The following values have to be calculated by the L0TP-packet
	 * L0TP_RAW is to be defined
	 */
//	l0::MEPEvent* L0TPEvent = event->getL0TPSubevent()->getPart(0);
//	L0TP_RAW* L0TPData = (L0TP_RAW*) L0TPEvent->getData();
//	event->setFinetime(L0TPData->fineTime);
	event->setProcessingID(0); // 0 indicates raw data as collected from the detector

	if (rr++ % L1_DOWNSCALE_FACTOR == 0) {
		// Accept event
		return 0x0101;
	}
	return 0;
}
} /* namespace na62 */
