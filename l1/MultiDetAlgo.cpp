/*
 * MultiDetAlgo.cpp
 *
 *  Created on: March 5, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk 
 */

#include "MultiDetAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <string.h>

#include <options/Logging.h>

#include "../common/decoding/TrbFragmentDecoder.h"

namespace na62 {

MultiDetAlgo::MultiDetAlgo() {
}

MultiDetAlgo::~MultiDetAlgo() {
// TODO Auto-generated destructor stub
}

uint_fast8_t MultiDetAlgo::processMultiDetTrigger(DecoderHandler& decoder) {

//	using namespace l0;
//
//	uint noEdgesPerTrb_cedar[decoder.getNumberOfCEDARFragments()];
//	uint noEdgesPerTrb_chod[decoder.getNumberOfCHODFragments()];
//
//	uint nEdges_cedar_tot = 0;
//	uint nEdges_chod_tot = 0;
//
////	LOG_INFO<< "Event number = " << event->getEventNumber() << ENDL;
////	LOG_INFO<< "Timestamp = " << std::hex << event->getTimestamp() << std::dec << ENDL;
//
//	for (TrbFragmentDecoder* cedarPacket : decoder.getCEDARDecoderRange()) {
//
//		noEdgesPerTrb_cedar[cedarPacket->getFragmentNumber()] = cedarPacket->getNumberOfEdgesStored();
////		LOG_INFO<< "KTAG: Tel62 ID " << trbNum << " - Number of Edges found " << noEdgesPerTrb_cedar[trbNum] << ENDL;
//
//		nEdges_cedar_tot += noEdgesPerTrb_cedar[cedarPacket->getFragmentNumber()];
//	}
//
//	for (TrbFragmentDecoder* chodPacket : decoder.getCHODDecoderRange()) {
//
//		noEdgesPerTrb_chod[chodPacket->getFragmentNumber()] = chodPacket->getNumberOfEdgesStored();
////		LOG_INFO<< "CHOD: Tel62 ID " << trbNum << " - Number of Edges found " << noEdgesPerTrb_chod[trbNum] << ENDL;
//
//		nEdges_chod_tot += noEdgesPerTrb_chod[chodPacket->getFragmentNumber()];
//	}
//
//	//LOG_INFO<<"MultiDetAlgo.cpp: Analysed Event " << event->getEventNumber() << " - Total Number of edges found (KTAG) " << nEdges_cedar_tot << ENDL;
//	//LOG_INFO<<"MultiDetAlgo.cpp: Analysed Event " << event->getEventNumber() << " - Total Number of edges found (CHOD) " << nEdges_chod_tot << ENDL;
//
//	uint_fast8_t multi_det_trigger = 0;
//
//	if (nEdges_cedar_tot > 3 && nEdges_chod_tot > 10) {
//		return multi_det_trigger = 1;
//	} else
		return 0;

}

} /* namespace na62 */

