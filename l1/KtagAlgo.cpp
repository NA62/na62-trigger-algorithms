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
uint_fast8_t KtagAlgo::processKtagTrigger(DecoderHandler& decoder,
		L1InfoToStorage* l1Info) {

	using namespace l0;
//  LOG_INFO("Initial Time " << time[0].tv_sec << " " << time[0].tv_usec);
//	LOG_INFO("KTAGAlgo: event timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

	averageCHODHitTime = l1Info->getCHODAverageTime();
//	LOG_INFO("KTAGAlgo: chodtime " << averageCHODHitTime);

	bool isCHODRefTime;
	if (l1Info->isL1CHODProcessed() && averageCHODHitTime != -1.0e+28)
		isCHODRefTime = 1;
	else
		isCHODRefTime = 0;

	uint sector_occupancy_chod[8] = { 0 };
	uint sector_occupancy_l0tp[8] = { 0 };

	uint nEdges_tot = 0;

//TODO: chkmax need to be USED
	DecoderRange<TrbFragmentDecoder> x = decoder.getCEDARDecoderRange();
	if (x.begin() == x.end()) {
		LOG_ERROR("KTAG: Empty decoder range!");
		return 0;
	}

	for (TrbFragmentDecoder* cedarPacket : decoder.getCEDARDecoderRange()) {
//      LOG_INFO("First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec);

		if (!cedarPacket->isReady() || cedarPacket->isBadFragment()) {
			LOG_ERROR("KTAGAlgo: This looks like a Bad Packet!!!! ");
			return 0;
		}

		/**
		 * Get Arrays with hit Info
		 */
		const uint64_t* const edge_times = cedarPacket->getTimes();
//		const uint_fast8_t* const edge_chIDs = cedarPacket->getChIDs();
		const bool* const edge_IDs = cedarPacket->getIsLeadings();
		const uint_fast8_t* const edge_tdcIDs = cedarPacket->getTdcIDs();
		double finetime, edgetime, dt_l0tp, dt_chod;

		uint numberOfEdgesOfCurrentBoard = cedarPacket->getNumberOfEdgesStored();

//		LOG_INFO("Tel62 ID " << cedarPacket->getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard);
//      LOG_INFO("time check (inside iterator - end of retrieve) " << time[2].tv_sec << " " << time[2].tv_usec);

//		LOG_INFO("Before loop on the edges KTag");
		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//			LOG_INFO("Edge " << iEdge << " ID " << edge_IDs[iEdge]);
//			LOG_INFO("Edge " << iEdge << " chID " << (uint) edge_chIDs[iEdge]);
//			LOG_INFO("Edge " << iEdge << " tdcID " << (uint) edge_tdcIDs[iEdge]);
//			LOG_INFO("Edge " << iEdge << " time " << std::hex << edge_times[iEdge] << std::dec);
//			LOG_INFO(decoder.getDecodedEvent()->getFinetime());
//			LOG_INFO(decoder.getDecodedEvent()->getBurstID());

			/**
			 * Process leading edges only
			 *
			 */
			if (edge_IDs[iEdge]) {
				finetime = decoder.getDecodedEvent()->getFinetime() * 0.097464731802;
				edgetime = (edge_times[iEdge] - decoder.getDecodedEvent()->getTimestamp() * 256.) * 0.097464731802;
//				LOG_INFO("finetime (in ns) " << finetime << " edgetime (in ns) " << edgetime);

				dt_l0tp = fabs(edgetime - finetime);

				if(isCHODRefTime) dt_chod = fabs(edgetime - averageCHODHitTime);
				else dt_chod = -1.0e+28;
//				LOG_INFO("dt_l0tp " << dt_l0tp << " dt_chod " << dt_chod);

				if (dt_l0tp < 10. || (isCHODRefTime && dt_chod < 10.)) {
					const uint trbID = edge_tdcIDs[iEdge] / 4;
					const uint box = calculateSector(cedarPacket->getFragmentNumber(), trbID);
//					LOG_INFO("box " << box);

					if (dt_l0tp < 10.) sector_occupancy_l0tp[box]++;
					if (isCHODRefTime && dt_chod < 10.) sector_occupancy_chod[box]++;
//				LOG_INFO("ANGELA-L1" << "\t" << decoder.getDecodedEvent()->getEventNumber() << "\t" << decoder.getDecodedEvent()->getTimestamp() << "\t" << (int)edge_IDs[iEdge] << "\t" << (int)edge_tdcIDs[iEdge] << "\t" << edge_times[iEdge] << "\t" << trbID << "\t" << box);
				}
			}
		}
//      LOG_INFO("time check (inside iterator - end of computations) " << time[3].tv_sec << " " << time[3].tv_usec);
		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}

//	LOG_INFO("KtagAlgo.cpp: Analysing Event " << decoder.getDecodedEvent()->getEventNumber() << " - Timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << " - Total Number of edges found " << nEdges_tot);
//	LOG_INFO(decoder.getDecodedEvent()->getFinetime());
//	LOG_INFO(decoder.getDecodedEvent()->getBurstID());

//  LOG_INFO("time check (outside for - all Tel62s)" << time[4].tv_sec << " " << time[4].tv_usec);

	uint nSectors_chod = 0;
	uint nSectors_l0tp = 0;
	for (uint iSec = 0; iSec != 8; iSec++) {
		if (sector_occupancy_l0tp[iSec])
			nSectors_l0tp++;
		if (isCHODRefTime && sector_occupancy_chod[iSec])
			nSectors_chod++;
	}
//	LOG_INFO("sectors_l0tp = " << nSectors_l0tp << " sectors_chod = " << nSectors_chod);
//	LOG_INFO("Angela: " << decoder.getDecodedEvent()->getEventNumber() << "\t" << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

//  LOG_INFO("time check (outside for - Nsector coincidences)" << time[5].tv_sec << " " << time[5].tv_usec);
//for (int i = 0; i < 5; i++) {
//             if(i && i!=3) LOG_INFO(((time[i+1].tv_sec - time[i].tv_sec)*1e6 + time[i+1].tv_usec) - time[i].tv_usec << " ");
//     }

//	LOG_INFO(std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << " " << nEdges_tot << " " << nSectors << " " << ((time[4].tv_sec - time[0].tv_sec)*1e6 + time[4].tv_usec) - time[0].tv_usec << " " << ((time[5].tv_sec - time[0].tv_sec)*1e6 + time[5].tv_usec) - time[0].tv_usec);
//	LOG_INFO(nEdges_tot << " " << ((time[4].tv_sec - time[0].tv_sec)*1e6 + time[4].tv_usec) - time[0].tv_usec << " " << ((time[5].tv_sec - time[0].tv_sec)*1e6 + time[5].tv_usec) - time[0].tv_usec);

	return ((nSectors_l0tp > 4) || (isCHODRefTime && nSectors_chod > 4));
//	return (nSectors_l0tp > 4);
}

}
/* namespace na62 */

