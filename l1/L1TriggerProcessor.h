/*
 * TriggerProcessor.h
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#pragma once
#ifndef TRIGGERPROCESSOR_H_
#define TRIGGERPROCESSOR_H_

#include <sys/types.h>
#include <atomic>
#include <cstdint>

namespace na62 {
  
  class EventBuilder;
  class Event;
  class CedarData;
  class KtagAlgo;

  class L1TriggerProcessor {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	//L1TriggerProcessor();
	//virtual ~L1TriggerProcessor();
	static uint8_t compute(Event* event);

private:

};

} /* namespace na62 */
#endif /* TRIGGERPROCESSOR_H_ */
