/*
 * LAVAlgo.cpp
 *
 *  Created on: 7 Sep 2015
 *      Author: romano
 */
#include "LAVAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#include <options/Logging.h>

#include "../common/decoding/TrbFragmentDecoder.h"
#include "lav_algorithm/ParsConfFile.h"

namespace na62 {

LAVParsConfFile* LAVAlgo::infoLAV_ = LAVParsConfFile::GetInstance();
int * LAVAlgo::lgGeo = infoLAV_->getGeoLGMap();
int LAVAlgo::hit[maxNROchs];
uint LAVAlgo::nHits;
double LAVAlgo::averageCHODHitTime = 0.;

LAVAlgo::LAVAlgo() {
}

LAVAlgo::~LAVAlgo() {
// TODO Auto-generated destructor stub
}

uint_fast8_t LAVAlgo::processLAVTrigger(DecoderHandler& decoder,
		L1InfoToStorage* l1Info) {

	using namespace l0;
//	LOG_INFO<< "Initial Time " << time[0].tv_sec << " " << time[0].tv_usec << ENDL;
//	LOG_INFO<< "LAVAlgo: event timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << ENDL;

	averageCHODHitTime = l1Info->getCHODAverageTime();
//	LOG_INFO<< "LAVAlgo: chodtime " << averageCHODHitTime << ENDL;

	bool isCHODRefTime;
	if (l1Info->isL1CHODProcessed() && averageCHODHitTime != -1.0e+28)
		isCHODRefTime = 1;
	else
		isCHODRefTime = 0;

	nHits = 0;
	uint nEdges_tot = 0;

	//TODO: chkmax need to be USED
	DecoderRange<TrbFragmentDecoder> x = decoder.getLAVDecoderRange();
	if (x.begin() == x.end()) {
		LOG_ERROR<< "LAV: Empty decoder range!";
		return 0;
	}

	for (TrbFragmentDecoder* lavPacket : decoder.getLAVDecoderRange()) {
//	LOG_INFO<< "First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec << ENDL;

		if (!lavPacket->isReady() || lavPacket->isBadFragment()) {
			LOG_ERROR<< "LAVAlgo: This looks like a Bad Packet!!!! " << ENDL;
			return 0;
		}

		for (uint i = 0; i != maxNROchs; i++) {
			hit[i] = 0;
		}

		/**
		 * Get Arrays with hit Info
		 */
		const uint64_t* const edge_times = lavPacket->getTimes();
		const uint_fast8_t* const edge_chIDs = lavPacket->getChIDs();
		const bool* const edge_IDs = lavPacket->getIsLeadings();
		const uint_fast8_t* const edge_tdcIDs = lavPacket->getTdcIDs();
		const uint_fast16_t edge_trbIDs = lavPacket->getFragmentNumber();
		double finetime, edgetime, dt_l0tp, dt_chod;

		uint numberOfEdgesOfCurrentBoard = lavPacket->getNumberOfEdgesStored();

//		LOG_INFO<< "LAV: Tel62 ID " << lavPacket->getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard << ENDL;
//		LOG_INFO<< "Reference detector fine time " << decoder.getDecodedEvent()->getFinetime() << ENDL;

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//			LOG_INFO<< "Edge " << iEdge << " ID " << edge_IDs[iEdge] << ENDL;
//			LOG_INFO<< "Edge " << iEdge << " chID " << (uint) edge_chIDs[iEdge] << ENDL;
//			LOG_INFO<< "Edge " << iEdge << " tdcID " << (uint) edge_tdcIDs[iEdge] << ENDL;
//			LOG_INFO<< "Edge " << iEdge << " time " << std::hex << edge_times[iEdge] << std::dec << ENDL;

//			if (edge_IDs[iEdge]) {
			const int roChID = edge_trbIDs * 512 + edge_tdcIDs[iEdge] * 32 + edge_chIDs[iEdge];
//			LOG_INFO<< "Readout Channel ID " << roChID << ENDL;
//			LOG_INFO<< "Geom LeadGlass ID " << lgGeo[roChID] << ENDL;

			const int roChIDPerTrb = edge_tdcIDs[iEdge] * 32 + edge_chIDs[iEdge];
//			LOG_INFO<< "Readout Channel ID per Tel62 " << roChIDPerTrb << ENDL;

			if ((roChID & 1) == 0) {
				finetime = decoder.getDecodedEvent()->getFinetime() * 0.097464731802;
				edgetime = (edge_times[iEdge] - decoder.getDecodedEvent()->getTimestamp() * 256.) * 0.097464731802;
//				LOG_INFO<< "finetime (in ns) " << finetime << ENDL;
//				LOG_INFO<< "edgetime (in ns) " << edgetime << ENDL;

				dt_l0tp = fabs(edgetime - finetime);

				if(isCHODRefTime) dt_chod = fabs(edgetime - averageCHODHitTime);
				else dt_chod = -1.0e+28;
//				LOG_INFO<< "dt_l0tp " << dt_l0tp << " dt_chod " << dt_chod << ENDL;

//				if (dt_l0tp < 20.) {
				if (dt_l0tp < 20. || (isCHODRefTime && dt_chod < 20.)) {
					if (edge_IDs[iEdge]) {
						hit[roChIDPerTrb]++;
//						LOG_INFO<< "Increment hit[" << roChIDPerTrb << "] to " << hit[roChIDPerTrb] << ENDL;
					}
					else if (hit[roChIDPerTrb]) {
						nHits++;
//						LOG_INFO<< "Increment nHits " << nHits << ENDL;
						if (nHits >= 3) return 0;
					}
				}
			} else {
//				printf("ODD! - Soglia alta! - Out Of Time! \n");
			}
		}
//	LOG_INFO<< "time check " << time[2].tv_sec << " " << time[2].tv_usec << ENDL;
		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}

//	LOG_INFO<<"LAVAlgo.cpp: Analysed Event " << decoder.getDecodedEvent()->getEventNumber() << " - nEdges_tot " << nEdges_tot << " - nHits " << nHits << ENDL;
//	LOG_INFO<< "time check (final)" << time[3].tv_sec << " " << time[3].tv_usec << ENDL;

//	for (int i = 0; i < 3; i++) {
//		if (i)
//			LOG_INFO<<((time[i+1].tv_sec - time[i].tv_sec)*1e6 + time[i+1].tv_usec) - time[i].tv_usec << " ";
//		}
//	LOG_INFO<< ((time[3].tv_sec - time[0].tv_sec)*1e6 + time[3].tv_usec) - time[0].tv_usec << ENDL;

	return (nHits < 3);

}

}
/* namespace na62 */

