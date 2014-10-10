/*
 * StrawData.cpp
 *
 *  Created on: Sep 24, 2014
 *      Author: jpinzino
 */

#include "StrawData.h"

#include <eventBuilding/Event.h>
#include <l0/MEPFragment.h>
#include <stdlib.h>

namespace na62
    {

    StrawData::StrawData()
	{
	nhits = 0;
	hits = 0;
	strawHeader = 0;
	hitCounters = 0;
	}

    void StrawData::SetHits(l0::MEPFragment* srbDataFragment)
	{

	strawHeader = (SrbDataHeader*) srbDataFragment->getPayload();

	hitCounters = (uint8_t*) (srbDataFragment->getPayload() + strawHeader->packetLength - 16);

	hits = (SrbHits*) (srbDataFragment->getPayload() + 8);

	//coarseTime = *((uint*) (srbDataFragment->getPayload() + 4));

	nhits = (strawHeader->packetLength - 16 - 8) / 2;

	}

    }

