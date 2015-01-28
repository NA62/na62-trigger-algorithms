/*
 * L1TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L1TriggerProcessor.h"
#include <options/Logging.h>

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>

#include "KtagAlgo.h"

namespace na62 {


  //L1TriggerProcessor::L1TriggerProcessor(){}
  //L1TriggerProcessor::~L1TriggerProcessor(){}
  
uint8_t L1TriggerProcessor::compute(Event* event) {
  using namespace l0;
  using namespace std;

  uint8_t trigger = KtagAlgo::checkKtagTrigger(event);
  if(trigger){
		LOG_INFO<<"event number = " << event->getEventNumber() << ENDL;
    LOG_INFO  << "GOOD EVENT! " << ENDL;
    return trigger;
  }
  else LOG_INFO  << "BAD EVENT! " << ENDL;

  //event->setProcessingID(0); // 0 indicates raw data as collected from the detector 
  //return 1;

}



} /* namespace na62 */
