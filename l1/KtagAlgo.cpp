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

#include <sys/time.h>

#include "../common/decoding/DecoderRange.h"
#include "../common/decoding/DecoderHandler.h"
#include "../common/decoding/TrbFragmentDecoder.h"

#define maxNhits 500

namespace na62 {

uint_fast8_t KtagAlgo::processKtagTrigger(DecoderHandler& decoder) {
	struct timeval time[10];

	gettimeofday(&time[0], 0);
//  LOG_INFO<< "Initial Time " << time[0].tv_sec << " " << time[0].tv_usec << ENDL;

	using namespace l0;

	uint sector_occupancy[8] = { 0 };

	uint nEdges_tot = 0;

//	LOG_INFO<< "Event number = " << event->getEventNumber() << ENDL;
//	LOG_INFO<< "Timestamp = " << std::hex << event->getTimestamp() << std::dec << ENDL;

//TODO: chkmax need to be USED

	for (TrbFragmentDecoder* cedarPacket : decoder.getCEDARDecoderRange()) {
		gettimeofday(&time[1], 0);
//      LOG_INFO<< "First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec << ENDL;

		/**
		 * Get Arrays with hit Info
		 */
//		const uint64_t* const edge_times = cedarPacket->getTimes();
//		const uint_fast8_t* const edge_chIDs = cedarPacket->getChIDs();
		const bool* const edge_IDs = cedarPacket->getIsLeadings();
		const uint_fast8_t* const edge_tdcIDs = cedarPacket->getTdcIDs();

		uint numberOfEdgesOfCurrentBoard =
				cedarPacket->getNumberOfEdgesStored();

//		LOG_INFO<< "Tel62 ID " << cedarPacket->getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard << ENDL;
		gettimeofday(&time[2], 0);
//      LOG_INFO<< "time check (inside iterator - end of retrieve) " << time[2].tv_sec << " " << time[2].tv_usec << ENDL;

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//			LOG_INFO<< "Edge " << iEdge << " ID " << edge_IDs[iEdge] << ENDL;
//			LOG_INFO<< "Edge " << iEdge << " chID " << (uint) edge_chIDs[iEdge] << ENDL;
//			LOG_INFO<< "Edge " << iEdge << " tdcID " << (uint) edge_tdcIDs[iEdge] << ENDL;
//			LOG_INFO<< "Edge " << iEdge << " time " << std::hex << edge_times[iEdge] << std::dec << ENDL;

//			LOG_INFO<< "pp[" << iEdge + nEdges_tot << "] " << pp[iEdge + nEdges_tot] << ENDL;

			if (edge_IDs[iEdge]) {
				const uint trbID = edge_tdcIDs[iEdge] / 4;
				const uint box = calculateSector(
						cedarPacket->getFragmentNumber(), trbID);

//				LOG_INFO << "box " << box << ENDL;
				sector_occupancy[box]++;
			}
			//LOG_INFO<< "ANGELA-L1" << "\t" << decoder.getDecodedEvent()->getEventNumber() << "\t" << decoder.getDecodedEvent()->getTimestamp() << "\t" << (int)edge_IDs[iEdge] << "\t" << (int)edge_chIDs[iEdge]<< "\t" << (int)edge_tdcIDs[iEdge] << "\t" << edge_times[iEdge] << "\t" << trbID << "\t" << box << ENDL;
		}
		gettimeofday(&time[3], 0);
//      LOG_INFO<< "time check (inside iterator - end of computations) " << time[3].tv_sec << " " << time[3].tv_usec << ENDL;

		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}

//	LOG_INFO<<"KtagAlgo.cpp: Analysing Event " << decoder.getDecodedEvent()->getEventNumber() << " - Timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << " - Total Number of edges found " << nEdges_tot << ENDL;
	gettimeofday(&time[4], 0);
//  LOG_INFO<< "time check (outside for - all Tel62s)" << time[4].tv_sec << " " << time[4].tv_usec << ENDL;

	uint nSectors = 0;
	for (uint iSec = 0; iSec != 8; iSec++) {
		if (sector_occupancy[iSec])
			nSectors++;
	}

//	LOG_INFO<< "Angela: " << decoder.getDecodedEvent()->getEventNumber() << "\t" << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << "\t" << nSectors << ENDL;
	gettimeofday(&time[5], 0);
//  LOG_INFO<< "time check (outside for - Nsector coincidences)" << time[5].tv_sec << " " << time[5].tv_usec << ENDL;
	//for (int i = 0; i < 5; i++) {
	//             if(i && i!=3) LOG_INFO<< ((time[i+1].tv_sec - time[i].tv_sec)*1e6 + time[i+1].tv_usec) - time[i].tv_usec << " ";
	//     }

//	LOG_INFO<< std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << " " << nEdges_tot << " " << nSectors << " " << ((time[4].tv_sec - time[0].tv_sec)*1e6 + time[4].tv_usec) - time[0].tv_usec << " " << ((time[5].tv_sec - time[0].tv_sec)*1e6 + time[5].tv_usec) - time[0].tv_usec << ENDL;
//	LOG_INFO<< nEdges_tot << " " << ((time[4].tv_sec - time[0].tv_sec)*1e6 + time[4].tv_usec) - time[0].tv_usec << " " << ((time[5].tv_sec - time[0].tv_sec)*1e6 + time[5].tv_usec) - time[0].tv_usec << ENDL;
	return nSectors > 3;
}

} /* namespace na62 */

