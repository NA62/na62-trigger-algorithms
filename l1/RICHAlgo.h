/*
 * RICHAlgo.h
 *
 *  Created on: 17 Apr 2015
 *      Author: Valeria Fascianelli
 */
#pragma once
#ifndef L1TRIGGERPROCESSOR_RICHALGO_H_
#define L1TRIGGERPROCESSOR_RICHALGO_H_

#include <sys/types.h>
#include <cstdint>

#include "../common/decoding/DecoderHandler.h"

namespace na62 {

class Event;

class RICHAlgo {
public:


	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */
	static uint_fast8_t processRICHTrigger(DecoderHandler& decoder);

private:

};

} /* namespace na62 */

#endif /* L1TRIGGERPROCESSOR_RICHALGO_H_ */
