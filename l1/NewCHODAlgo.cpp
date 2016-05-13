/*
 * NewCHODAlgo.cpp
 *
 *  Created on: 13 May 2016
 *      Author: lorenza
 */

#include "NewCHODAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#include <options/Logging.h>

#include "../common/decoding/TrbFragmentDecoder.h"

namespace na62 {

NewCHODParsConfFile* NewCHODAlgo::infoNewCHOD_ = NewCHODParsConfFile::GetInstance();
int * NewCHODAlgo::pmtGeo = infoNewCHOD_->getGeoPMTMap();
//uint NewCHODAlgo::nHits;
//uint NewCHODAlgo::nMaxPMTs;
uint NewCHODAlgo::pmtID1;
uint NewCHODAlgo::pmtID2;
int NewCHODAlgo::roChID;

NewCHODAlgo::NewCHODAlgo() {
}

NewCHODAlgo::~NewCHODAlgo() {
// TODO Auto-generated destructor stub
}

uint_fast8_t NewCHODAlgo::processNewCHODTrigger(DecoderHandler& decoder) {

	using namespace l0;

	//nMaxPMTs = 0;
	//nHits = 0;

	LOG_INFO("Event number = " << decoder.getDecodedEvent()->getEventNumber());
	LOG_INFO("Timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

	TrbFragmentDecoder& newchodPacket =
			(TrbFragmentDecoder&) decoder.getDecodedNewCHODFragment(0);
	/**
	 * Get Arrays with hit Info
	 */

	const uint64_t* const edge_times = newchodPacket.getTimes();
	const uint_fast8_t* const edge_chIDs = newchodPacket.getChIDs();
	const bool* const edge_IDs = newchodPacket.getIsLeadings();
	const uint_fast8_t* const edge_tdcIDs = newchodPacket.getTdcIDs();
	double finetime, edgetime;

	uint numberOfEdgesOfCurrentBoard = newchodPacket.getNumberOfEdgesStored();
//	LOG_INFO("newchod: Tel62 ID " << newchodPacket.getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard);

	for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {

		edgetime = (edge_times[iEdge]
				- decoder.getDecodedEvent()->getTimestamp() * 256.)
				* 0.097464731802;

//		LOG_INFO("finetime (decoder) " << (uint)decoder.getDecodedEvent()->getFinetime());
//		LOG_INFO("edge_time " << std::hex << edge_times[iEdge] << std::dec);
		LOG_INFO("finetime (in ns) " << finetime);
		LOG_INFO("edgetime (in ns) " << edgetime);
//		LOG_INFO("Without offset " << fabs(edgetime - finetime));

		/**
		 * Process leading edges only
		 *
		 */
		if (edge_IDs[iEdge]) {
			roChID = (edge_tdcIDs[iEdge] * 32) + edge_chIDs[iEdge];
			if (pmtGeo[roChID] > 0) pmtID1 = pmtGeo[roChID] - 50;
			else pmtID1 = pmtGeo[roChID];
			for (uint jEdge = 0; jEdge != numberOfEdgesOfCurrentBoard; jEdge++) {
				if (edge_IDs[jEdge] && jEdge != iEdge) {
					roChID = (edge_tdcIDs[jEdge] * 32) + edge_chIDs[jEdge];
					if (pmtGeo[roChID] > 151) pmtID2 = pmtGeo[roChID] - 50;
					else pmtID2 = pmtGeo[roChID];
				}
			}
		}
	}
}
}
/* namespace na62 */


