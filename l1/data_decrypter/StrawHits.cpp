/*
 * StrawHits.cpp
 *
 *  Created on: Sep 25, 2014
 *      Author: jpinzino
 */

#include "StrawHits.h"
#include "StrawData.h"
#include "../straw_algorithm/cut.h"
#include <stdlib.h>

namespace na62
    {

    PlaneHit::PlaneHit()
	{
	halfViewZero = new StrawHits[1];
	halfViewOne = new StrawHits[1];
	}
    PlaneHit::~PlaneHit()
	{
	delete halfViewZero;
	delete halfViewOne;
	}

    void PlaneHit::SetHits(StrawData data, uint32_t triggerCoarse, uint8_t triggerFine)
	{

	if (data.nhits > 0)
	    {
	    int ncounters = 0;
	    uint nhits_temp = 0;
	    uint16_t triggerCombined = 0;
	    uint16_t timeTemp = 0;

	    for (int nslot = 0; nslot < 16; nslot++)
		{
		ncounters = (int) (data.hitCounters[nslot]);
		triggerCombined = (uint16_t) ((triggerCoarse << 5) | (triggerFine >> 3));
		timeTemp = ((data.strawHeader->coarseTime) + nslot)<<5;

		if (((data.strawHeader->coarseTime) + nslot - triggerCoarse) > 0 && ((data.strawHeader->coarseTime) + nslot - triggerCoarse) < 11)
		    {
		    for (int i = 0; i < ncounters; i++)
			{
			if (data.hits[nhits_temp].strawID < strawSeparator)
			    halfViewZero->SetHits(data.hits[nhits_temp], triggerCombined, timeTemp);
			else
			    halfViewOne->SetHits(data.hits[nhits_temp], triggerCombined, timeTemp);

			nhits_temp++;
			}
		    }

		}

	    }
	}

    StrawHits::StrawHits()
	{
	nhit = 0;
	strawID = (uint8_t*) calloc(maxNhit, sizeof(uint8_t));
	leading = (uint16_t*) calloc(maxNhit, sizeof(uint16_t));
	trailing = (uint16_t*) calloc(maxNhit, sizeof(uint16_t));
	used = (uint*) calloc(maxNhit, sizeof(uint16_t));

	}

    StrawHits::~StrawHits()
	{
	// TODO Auto-generated destructor stub
	free(strawID);
	free(leading);
	free(trailing);
	free(used);
	}

    void StrawHits::SetHits(SrbHits hit, uint16_t trigger, uint16_t timeTemp)
	{

	int temp=0;
	int initialNhit = nhit;

	if (nhit == 0)
	    {
	    strawID[0] =hit.strawID;
	    if ((int) hit.edge == 0) //0 leading, 1 trailing
		leading[0] = (uint16_t) ((timeTemp | hit.fineTime)-trigger);
	    else
		trailing[0] = (uint16_t) ((timeTemp | hit.fineTime)-trigger);

	    nhit++;

	    }
	else
	    {
	    temp = 0;
	    for (int j = initialNhit; j < nhit; j++)
		{
		if ((int) hit.strawID == strawID[j])
		    {
		    temp = 1;
		    if ((int) hit.edge == 0 && (hit.fineTime < leading[j] || leading[j] == 0)) //0 leading
			leading[j] = (uint16_t) ((timeTemp | hit.fineTime)-trigger);
		    else if (hit.fineTime > trailing[j]) //1 trailing
			trailing[j] = (uint16_t) ((timeTemp | hit.fineTime)-trigger);
		    }
		}
	    if (temp == 0)
		{
		strawID[nhit] = (int) hit.strawID;

		if ((int) hit.edge == 0)
		    leading[nhit] = (uint16_t) ((timeTemp | hit.fineTime)-trigger);
		else
		    trailing[nhit] = (uint16_t) ((timeTemp | hit.fineTime)-trigger);

		nhit++;
		}

	    }

	}

    }
