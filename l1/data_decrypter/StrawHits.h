/*
 * StrawHits.h
 *
 *  Created on: Sep 25, 2014
 *      Author: jpinzino
 */

#ifndef STRAWHITS_H_
#define STRAWHITS_H_

#include <sys/types.h>
#include <cstdint>

namespace na62
    {
    class StrawData;
    class SrbHits;
    }

namespace na62
    {

    class StrawHits
	{
    public:
	int nhit;
	uint8_t* strawID;
	uint16_t* leading;
	uint16_t* trailing;
	uint* used;

	StrawHits();
	virtual ~StrawHits();

	void SetHits(SrbHits, uint16_t, uint16_t);
	};

    class PlaneHit
	{
    public:

	StrawHits* halfViewZero;
	StrawHits* halfViewOne;

	PlaneHit();
	virtual ~PlaneHit();

	void SetHits(StrawData, uint32_t, uint8_t);

	};

    }
#endif /* STRAWHITS_H_ */
