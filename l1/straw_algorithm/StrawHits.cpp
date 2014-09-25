/*
 * strawalgorithm.cpp
 *
 *  Created on: Sep 24, 2014
 *      Author: jpinzino
 */

#include "StrawHits.h"

#include <eventBuilding/Event.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>

namespace na62 {

StrawHits::StrawHits() {
	nhits = 0;
	hits = 0;
	strawHeader = 0;
}

void StrawHits::SetHits(Event* event) {

	l0::Subevent* strawSubevent = event->getSTRAWSubevent();

	for (int srbNum = 0; srbNum != strawSubevent->getNumberOfFragments();
			srbNum++) {
		l0::MEPFragment* srbDataFragment = strawSubevent->getFragment(srbNum);
		hits = (uint16_t*) (srbDataFragment->getPayload() + 8);

		//coarseTime = *((uint*) (srbDataFragment->getPayload() + 4));

		strawHeader = (SrbDataHeader*) srbDataFragment->getPayload();
		nhits = (strawHeader->packetLength - 16 - 8) / 2;

	}

}

}
