/*
 * StrawHits.h
 *
 *  Created on: Sep 24, 2014
 *      Author: jpinzino
 */

#ifndef STRAWALGORITM_H_
#define STRAWALGORITM_H_

#include <sys/types.h>
#include <cstdint>

namespace na62 {
class Event;
} /* namespace na62 */

namespace na62 {

struct SrbDataHeader {
	uint8_t ID :4;
	uint8_t flags :4;
	uint8_t triggerType;
	uint16_t packetLength;

	uint32_t coarseTime;
}__attribute__ ((__packed__));

class StrawHits {
public:
	uint nhits;
	uint16_t* hits;
	SrbDataHeader* strawHeader;

	StrawHits();
	void SetHits(Event*);

};

}



#endif /* STRAWALGORITM_H_ */
