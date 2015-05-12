/*
 * CHODAlgo.cpp
 *
 *  Created on: May 11, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk 
 */

#include "CHODAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <string.h>
#include <math.h>

#include <options/Logging.h>

#include "../common/decoding/TrbFragmentDecoder.h"
#include "chod_algorithm/ParsConfFile.h"

namespace na62 {

ParsConfFile* CHODAlgo::infoCHOD_ = ParsConfFile::GetInstance();
int * CHODAlgo::slabGeo = infoCHOD_->getGeoSlabMap();
uint CHODAlgo::nHits;
uint CHODAlgo::nMaxSlabs;
uint CHODAlgo::nCandidates;
int CHODAlgo::slabID[maxNhits];
int CHODAlgo::quadrantID[maxNhits];
int CHODAlgo::planeID[maxNhits];
uint64_t CHODAlgo::time[maxNhits];

CHODAlgo::CHODAlgo() {
}

CHODAlgo::~CHODAlgo() {
// TODO Auto-generated destructor stub
}

uint_fast8_t CHODAlgo::processCHODTrigger(DecoderHandler& decoder) {

	using namespace l0;

	uint nEdges_chod_tot = 0;
	nHits = 0;
	nMaxSlabs = 14;
	nCandidates = 0;

//	LOG_INFO<< "Event number = " << decoder.getDecodedEvent()->getEventNumber() << ENDL;
//	LOG_INFO<< "Timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << ENDL;

	for (TrbFragmentDecoder* chodPacket : decoder.getCHODDecoderRange()) {
		/**
		 * Get Arrays with hit Info
		 */
		const uint64_t* const edge_times = chodPacket->getTimes();
		const uint_fast8_t* const edge_chIDs = chodPacket->getChIDs();
		const bool* const edge_IDs = chodPacket->getIsLeadings();
		const uint_fast8_t* const edge_tdcIDs = chodPacket->getTdcIDs();

		uint numberOfEdgesOfCurrentBoard = chodPacket->getNumberOfEdgesStored();
		//LOG_INFO<< "CHOD: Tel62 ID " << chodPacket->getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard << ENDL;

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
			/**
			 * Process leading edges only
			 *
			 */
			if (edge_IDs[iEdge]) {
				const int roChID = (edge_tdcIDs[iEdge] * 32)
						+ edge_chIDs[iEdge];
//				LOG_INFO<< "Readout Channel ID " << roChID << ENDL;
//				LOG_INFO<< "Geom Slab ID " << slabGeo[roChID] << ENDL;

				/**
				 * Process only first 128 readout channels, corresponding to low-threshold LAV FEE
				 *
				 */
				if (slabGeo[roChID] < 128) {
//					LOG_INFO<< "Edge " << iEdge << " ID " << edge_IDs[iEdge] << ENDL;
//					LOG_INFO<< "Edge " << iEdge << " chID " << (uint) edge_chIDs[iEdge] << ENDL;
//					LOG_INFO<< "Edge " << iEdge << " tdcID " << (uint) edge_tdcIDs[iEdge] << ENDL;
//					LOG_INFO<< "Edge " << iEdge << " time " << std::hex << edge_times[iEdge] << std::dec << ENDL;

					slabID[nHits] = slabGeo[roChID];
					quadrantID[nHits] = slabID[nHits] / 16;
					planeID[nHits] = slabID[nHits] / 64;
					time[nHits] = edge_times[iEdge];

//					LOG_INFO<< "CHOD slab ID " << slabID[nHits] << ENDL;
//					LOG_INFO<< "CHOD quadrant ID " << quadrantID[nHits] << ENDL;
//					LOG_INFO<< "CHOD plane ID " << planeID[nHits] << ENDL;
//					LOG_INFO<< "CHOD time " << std::hex << time[nHits] << std::dec << ENDL;

					nHits++;
				}
			}
		}
		nEdges_chod_tot += numberOfEdgesOfCurrentBoard;
	}
//	LOG_INFO<<"CHODAlgo.cpp: Analysed Event " << decoder.getDecodedEvent()->getEventNumber() << " - Total Number of edges " << nEdges_chod_tot << " - nHits " << nHits << ENDL;

//     return nHits < nMaxSlabs;

//     LOG_INFO<< "Sto entrando nella trigger condition!!!!" << ENDL;
	for (uint iHit = 0; iHit != nHits; iHit++) {
		int counter1 = slabID[iHit];
		if (counter1 > 63)
			continue;
		int64_t time1 = time[iHit];
		int quadrant1 = quadrantID[iHit];
		int plane1 = planeID[iHit];
//		LOG_INFO<< "Hit1 " << counter1 << " " << quadrant1 << " " << plane1 << " " << std::hex << time1 << std::dec << ENDL;

		for (uint jHit = 0; jHit != nHits; jHit++) {
			if (jHit == iHit)
				continue;
			int counter2 = slabID[jHit];
			int64_t time2 = time[jHit];
			int quadrant2 = quadrantID[jHit];
			int plane2 = planeID[jHit];
//			LOG_INFO<< "Hit2 " << counter2 << " " << quadrant2 << " " << plane2 << " " << std::hex << time2 << std::dec << ENDL;

//Check if slabs have "intersections"
//			LOG_INFO<< "fabs(time1- time2) " << fabs(time1 - time2) << ENDL;
			if (fabs(time1 - time2) > 100)
				continue;

			// try to make a candidate
			if ((nHits < nMaxSlabs) && ((quadrant2 - quadrant1) == 4)
					&& (plane2 != plane1)) {
				nCandidates++;
			}
		}
	}

//	LOG_INFO<< "Number of CHOD candidates " << nCandidates << ENDL;
//	LOG_INFO<< nCandidates << ENDL;

	return nCandidates;

}

} /* namespace na62 */

