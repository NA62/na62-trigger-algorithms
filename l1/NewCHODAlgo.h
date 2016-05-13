/*
 * NewCHODAlgo.h
 *
 *  Created on: 13 May 2016
 *      Author: lorenza
 */

#ifndef NEWCHODALGO_H_
#define NEWCHODALGO_H_

#include <sys/types.h>
#include <cstdint>

#include "../common/decoding/DecoderHandler.h"
#include "newchod_algorithm/ParsConfFile.h"

namespace na62 {


class NewCHODAlgo {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	NewCHODAlgo();
	~NewCHODAlgo();
	static uint_fast8_t processNewCHODTrigger(DecoderHandler& decoder);

private:

	static NewCHODParsConfFile* infoNewCHOD_;
	static int* pmtGeo;
	//static uint nHits;
	//static uint nMaxPMTs;
	static uint pmtID1;
	static uint pmtID2;
	static int roChID;

};

} /* namespace na62 */

#endif /* NEWCHODALGO_H_ */
