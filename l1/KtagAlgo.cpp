/*
 * KtagAlgo.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk 
 */

#include "KtagAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <options/Logging.h>
#include <string.h>

#include "../common/decoding/DecoderRange.h"
#include "../common/decoding/DecoderHandler.h"
#include "../common/decoding/TrbFragmentDecoder.h"

#define maxNhits 500

namespace na62 {

uint_fast8_t KtagAlgo::processKtagTrigger(DecoderHandler& decoder) {
	using namespace l0;

	uint sector_occupancy[8];
	memset(sector_occupancy, 0, 8);

	uint nEdges_tot = 0;

	//LOG_INFO<< "Event number = " << event->getEventNumber() << ENDL;
	//LOG_INFO<< "Timestamp = " << std::hex << event->getTimestamp() << std::dec << ENDL;

	//TODO: chkmax need to be USED

//	for (TrbFragmentDecoder* cedarPacket : decoder.getCEDARDecoderRange()) {

	for (uint i = 0; i != decoder.getNumberOfCEDARFragments(); i++) {
		const TrbFragmentDecoder& tmp = decoder.getDecodedCEDARFragment(i);
		const TrbFragmentDecoder* cedarPacket = &tmp;
		/**
		 * Get Arrays with hit Info
		 */
//		const uint64_t* const edge_times = cedarPacket->getTimes();
//		const uint_fast8_t* const edge_chIDs = cedarPacket->getChIDs();
//		const bool* const edge_IDs = cedarPacket->getIsLeadings();
		const uint_fast8_t* const edge_tdcIDs = cedarPacket->getTdcIDs();

		uint noEdgesPerTrb = cedarPacket->getNumberOfEdgesStored();

		//LOG_INFO<< "Tel62 ID " << trbNum << " - Number of Edges found " << noEdgesPerTrb[trbNum] << ENDL;

		for (uint iEdge = 0; iEdge != noEdgesPerTrb; iEdge++) {
			//LOG_INFO<< "Edge " << iEdge + nEdges_tot << " ID " << edge_IDs[iEdge] << ENDL;
			//LOG_INFO<< "Edge " << iEdge + nEdges_tot << " chID " << edge_chIDs[iEdge] << ENDL;
			//LOG_INFO<< "Edge " << iEdge + nEdges_tot << " tdcID " << edge_tdcIDs[iEdge] << ENDL;
			//LOG_INFO<< "Edge " << iEdge + nEdges_tot << " time " << std::hex << edge_times[iEdge] << std::dec << ENDL;
			//LOG_INFO<< "Edge " << iEdge + nEdges_tot << " trbID " << edge_trbIDs[iEdge] << ENDL;

			//LOG_INFO<< "pp[" << iEdge + nEdges_tot << "] " << pp[iEdge + nEdges_tot] << ENDL;

			const uint trbID = edge_tdcIDs[iEdge] / 4;
			const uint box = calculateSector(cedarPacket->getFragmentNumber(),
					trbID);
			//LOG_INFO << "box[" << iEdge + nEdges_tot << "] " << box[iEdge + nEdges_tot] << ENDL;

			sector_occupancy[box]++;

			//LOG_INFO<< "ANGELA-L1" << "\t" << decoder.getDecodedEvent()->getEventNumber() << "\t" << decoder.getDecodedEvent()->getTimestamp() << "\t" << (int)edge_IDs[iEdge] << "\t" << (int)edge_chIDs[iEdge]<< "\t" << (int)edge_tdcIDs[iEdge] << "\t" << edge_times[iEdge] << "\t" << trbID << "\t" << box << ENDL;
			if (iEdge == (noEdgesPerTrb - 1)) {
				nEdges_tot += noEdgesPerTrb;
			}
		}
	}

	//LOG_INFO<<"KtagAlgo.cpp: Analysing Event " << event->getEventNumber() << " - Total Number of edges found " << nEdges_tot << ENDL;

	uint nSectors = 0;
	for (int iSec = 0; iSec != 8; iSec++) {
		if (sector_occupancy[iSec])
			nSectors++;
	}

	//LOG_INFO<< "Angela: " << decoder.getDecodedEvent()->getEventNumber() << "\t" << decoder.getDecodedEvent()->getTimestamp() << "\t" << nSectors << ENDL;

	return nSectors > 3;
}

} /* namespace na62 */

