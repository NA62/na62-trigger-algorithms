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
#include <math.h>

#include <sys/time.h>

#include "../common/decoding/DecoderRange.h"
#include "../common/decoding/DecoderHandler.h"
#include "../common/decoding/TrbFragmentDecoder.h"

#define maxNhits 500

namespace na62 {

double KtagAlgo::averageCHODHitTime = 0.;
uint_fast8_t KtagAlgo::processKtagTrigger(DecoderHandler& decoder,L1InfoToStorage* l1Info) {

//  LOG_INFO<< "Initial Time " << time[0].tv_sec << " " << time[0].tv_usec << ENDL;

	using namespace l0;

	averageCHODHitTime = l1Info->getCHODAverageTime();
//	LOG_INFO<< "PATchodtime " << averageCHODHitTime << ENDL;

	uint sector_occupancy_chod[8] = { 0 };
	uint sector_occupancy_l0tp[8] = { 0 };

	uint nEdges_tot = 0;

//TODO: chkmax need to be USED

	for (TrbFragmentDecoder* cedarPacket : decoder.getCEDARDecoderRange()) {
//      LOG_INFO<< "First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec << ENDL;

		/**
		 * Get Arrays with hit Info
		 */
		const uint64_t* const edge_times = cedarPacket->getTimes();
//		const uint_fast8_t* const edge_chIDs = cedarPacket->getChIDs();
		const bool* const edge_IDs = cedarPacket->getIsLeadings();
		const uint_fast8_t* const edge_tdcIDs = cedarPacket->getTdcIDs();
		double finetime, edgetime;

		uint numberOfEdgesOfCurrentBoard =
				cedarPacket->getNumberOfEdgesStored();

//		LOG_INFO<< "Tel62 ID " << cedarPacket->getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard << ENDL;
//      LOG_INFO<< "time check (inside iterator - end of retrieve) " << time[2].tv_sec << " " << time[2].tv_usec << ENDL;

//		LOG_INFO << "Before loop on the edges KTag" << ENDL;
		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//			LOG_INFO<< "Edge " << iEdge << " ID " << edge_IDs[iEdge] << ENDL;
//			LOG_INFO<< "Edge " << iEdge << " chID " << (uint) edge_chIDs[iEdge] << ENDL;
//			LOG_INFO<< "Edge " << iEdge << " tdcID " << (uint) edge_tdcIDs[iEdge] << ENDL;
//			LOG_INFO<< "Edge " << iEdge << " time " << std::hex << edge_times[iEdge] << std::dec << ENDL;
//			LOG_INFO<< decoder.getDecodedEvent()->getFinetime() << ENDL;
//			LOG_INFO<< decoder.getDecodedEvent()->getBurstID() << ENDL;

//			finetime = (uint64_t) (decoder.getDecodedEvent()->getFinetime()
//					+ (decoder.getDecodedEvent()->getTimestamp() << 8));
			finetime = decoder.getDecodedEvent()->getFinetime() * 0.097464731802;
			edgetime = (edge_times[iEdge]
					- decoder.getDecodedEvent()->getTimestamp() * 256.)
					* 0.097464731802;

//			LOG_INFO<< "finetime (decoder) " << (uint)decoder.getDecodedEvent()->getFinetime() << ENDL;
//			LOG_INFO<< "edge_time " << std::hex << edge_times[iEdge] << std::dec << ENDL;
//			LOG_INFO<< "PAT- finetime (in ns) " << finetime << " PATedgetime (in ns) " << edgetime << " PATT0time " << decoder.getDecodedEvent()->getFinetime() * 0.097464731802 << ENDL;
//			LOG_INFO<< "With offset " << fabs(edgetime + cedarOffsetFinetime - finetime) << ENDL;
			/**
			 * Process leading edges only
			 *
			 */
//			if (edge_IDs[iEdge]){
//			if (edge_IDs[iEdge] && fabs(edgetime - finetime) <= 10.) {

			double dt_chod = fabs(edgetime - averageCHODHitTime);
			double dt_l0tp = fabs(edgetime - finetime);
//			LOG_INFO<< "dt_l0tp " << dt_l0tp << " dt_chod " << dt_chod << ENDL;

			if (edge_IDs[iEdge] && (dt_l0tp < 10. || dt_chod < 10.)) {
				const uint trbID = edge_tdcIDs[iEdge] / 4;
				const uint box = calculateSector(
						cedarPacket->getFragmentNumber(), trbID);

// Algorithm for exercise with PATTI input
//const uint box = 2 * (edge_tdcIDs[iEdge] % 4) + (edge_chIDs[iEdge] / 8);
//				LOG_INFO << "box " << box << ENDL;
				if (dt_l0tp<10.) sector_occupancy_l0tp[box]++;
				if (dt_chod<10.) sector_occupancy_chod[box]++;
			}
//			LOG_INFO<< "ANGELA-L1" << "\t" << decoder.getDecodedEvent()->getEventNumber() << "\t" << decoder.getDecodedEvent()->getTimestamp() << "\t" << (int)edge_IDs[iEdge] << "\t" << (int)edge_chIDs[iEdge]<< "\t" << (int)edge_tdcIDs[iEdge] << "\t" << edge_times[iEdge] << "\t" << trbID << "\t" << box << ENDL;
		}
//      LOG_INFO<< "time check (inside iterator - end of computations) " << time[3].tv_sec << " " << time[3].tv_usec << ENDL;

		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}

//	LOG_INFO<<"KtagAlgo.cpp: Analysing Event " << decoder.getDecodedEvent()->getEventNumber() << " - Timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << " - Total Number of edges found " << nEdges_tot << ENDL;
//	LOG_INFO<<decoder.getDecodedEvent()->getFinetime() << ENDL;
//	LOG_INFO<<decoder.getDecodedEvent()->getBurstID() << ENDL;

//  LOG_INFO<< "time check (outside for - all Tel62s)" << time[4].tv_sec << " " << time[4].tv_usec << ENDL;

	uint nSectors_chod = 0;
	uint nSectors_l0tp = 0;
	for (uint iSec = 0; iSec != 8; iSec++) {
		if (sector_occupancy_l0tp[iSec]) nSectors_l0tp++;
		if (sector_occupancy_chod[iSec]) nSectors_chod++;
	}

//	LOG_INFO << "sectors_l0tp = " << nSectors_l0tp << " sectors_chod = " << nSectors_chod << ENDL;

//	LOG_INFO<< "Angela: " << decoder.getDecodedEvent()->getEventNumber() << "\t" << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << "\t" << nSectors << ENDL;

//  LOG_INFO<< "time check (outside for - Nsector coincidences)" << time[5].tv_sec << " " << time[5].tv_usec << ENDL;
//for (int i = 0; i < 5; i++) {
//             if(i && i!=3) LOG_INFO<< ((time[i+1].tv_sec - time[i].tv_sec)*1e6 + time[i+1].tv_usec) - time[i].tv_usec << " ";
//     }

//	LOG_INFO<< std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << " " << nEdges_tot << " " << nSectors << " " << ((time[4].tv_sec - time[0].tv_sec)*1e6 + time[4].tv_usec) - time[0].tv_usec << " " << ((time[5].tv_sec - time[0].tv_sec)*1e6 + time[5].tv_usec) - time[0].tv_usec << ENDL;
//	LOG_INFO<< nEdges_tot << " " << ((time[4].tv_sec - time[0].tv_sec)*1e6 + time[4].tv_usec) - time[0].tv_usec << " " << ((time[5].tv_sec - time[0].tv_sec)*1e6 + time[5].tv_usec) - time[0].tv_usec << ENDL;
//	return ((nSectors_l0tp > 4) || (nSectors_chod > 4));
	return (nSectors_l0tp > 4);
}

} /* namespace na62 */

