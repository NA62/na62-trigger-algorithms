/*
 * StrawData.h
 *
 *  Created on: Sep 24, 2014
 *      Author: jpinzino
 */

#ifndef STRAWALGORITM_H_
#define STRAWALGORITM_H_

#include <sys/types.h>
#include <cstdint>
#include <l0/MEPFragment.h>

//namespace na62 {
//class l0::MEPFragment;
//} /* namespace na62 */

namespace na62 {

struct SrbDataHeader {
	uint8_t SRBID :4;
	uint8_t flags :4;
	uint8_t triggerType;
	uint16_t packetLength;
	uint32_t coarseTime;
}__attribute__ ((__packed__));

struct SrbHits{
	uint8_t fineTime :5;
	uint8_t edge :1;
	uint8_t strawID :8;
	uint8_t err :2;
}__attribute__ ((__packed__));

class StrawData {
public:
	uint nhits;
	SrbHits* hits;
	SrbDataHeader* strawHeader;
	uint8_t* hitCounters;

	StrawData();
	void SetHits(l0::MEPFragment* );

};

}



#endif /* STRAWALGORITM_H_ */
