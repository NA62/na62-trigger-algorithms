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

uint L1TriggerProcessor::L1_DOWNSCALE_FACTOR = 0;
std::atomic<int> L1TriggerProcessor::rr(0);

int counter;
uint16_t L1TriggerProcessor::compute(Event* event) {
	std::cout << std::endl; // Neatens output display making testing easier
	StrawL1Algorithm strawAlgorithm(event);
	strawAlgorithm.compute();

//	long sum = 0;
//	for (int sourceIDNum = 0;
//			sourceIDNum != SourceIDManager::NUMBER_OF_L0_DATA_SOURCES; sourceIDNum++) {
//		Subevent* subevent = event->getL0SubeventBySourceIDNum(sourceIDNum);

//		Subevent* lav = event->getLAVSubevent();
//		for (int p = lav->getNumberOfParts() - 1; p >= 0; p--) {
//			MEPFragment* fragment = lav->getPart(p);
//			const MEPFragment_HDR* data = fragment->getData();
//
//			for(int i=0; i<fragment->getDataLength(); i++){
//				sum+=((char*)data)[i];
//			}
//		}

//	}
	/*
	 * Accessing the raw data of one Detector (MUV):
	 */
//	Subevent* muv = event->getMUVSubevent();
//	for (int p = muv->getNumberOfParts() - 1; p >= 0; p--) {
//		MEPEvent* mepData = muv->getPart(p);
//		mepData->getSourceID();
//		mepData->getData()
//		mepData->getEventLength()
//		mepData->getSourceIDNum()
//	} */
	/*
	 * The following values have to be calculated by the L0TP-packet
	 * L0TP_RAW is to be defined
	 */
//	l0::MEPEvent* L0TPEvent = event->getL0TPSubevent()->getPart(0);
//	L0TP_RAW* L0TPData = (L0TP_RAW*) L0TPEvent->getData();
//	event->setFinetime(L0TPData->fineTime);
	event->setProcessingID(0); // 0 indicates raw data as collected from the detector

//	return (sum%2)+1;

	if (rr++ % L1_DOWNSCALE_FACTOR == 0) {
		// Accept event
		return 0x0101;
	}
	return 0;
}

} /* namespace na62 */
