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
uint LAVAlgo::nHits;

LAVAlgo::LAVAlgo() {
}

LAVAlgo::~LAVAlgo() {
// TODO Auto-generated destructor stub
}

uint_fast8_t LAVAlgo::processLAVTrigger(DecoderHandler& decoder) {

	nHits = 0;

//	LOG_INFO<< "Initial Time " << time[0].tv_sec << " " << time[0].tv_usec << ENDL;

	using namespace l0;

	double LAVOffsetFinetime = 0.; //ns (from run 3015)

//	LOG_INFO<< "Event number = " << decoder.getDecodedEvent()->getEventNumber() << ENDL;
//	LOG_INFO<< "Timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << ENDL;

	TrbFragmentDecoder& LAVPacket =
			(TrbFragmentDecoder&) decoder.getDecodedLAVFragment(0);
//	LOG_INFO<< "First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec << ENDL;
	/**
	 * Get Arrays with hit Info
	 */
	const uint64_t* const edge_times = LAVPacket.getTimes();
	const uint_fast8_t* const edge_chIDs = LAVPacket.getChIDs();
	const bool* const edge_IDs = LAVPacket.getIsLeadings();
	const uint_fast8_t* const edge_tdcIDs = LAVPacket.getTdcIDs();
	double finetime, edgetime;

	uint numberOfEdgesOfCurrentBoard = LAVPacket.getNumberOfEdgesStored();
//	LOG_INFO<< "LAV: Tel62 ID " << LAVPacket.getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard << ENDL;
//	LOG_INFO<< "Reference detector fine time " << decoder.getDecodedEvent()->getFinetime() << ENDL;

	for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {

		finetime = decoder.getDecodedEvent()->getFinetime() * 0.097464731802;
		edgetime = (edge_times[iEdge]
				- decoder.getDecodedEvent()->getTimestamp() * 256.)
				* 0.097464731802;

//		LOG_INFO<< "finetime (decoder) " << (uint)decoder.getDecodedEvent()->getFinetime() << ENDL;
//		LOG_INFO<< "edge_time " << std::hex << edge_times[iEdge] << std::dec << ENDL;
//		LOG_INFO<< "finetime (in ns) " << finetime << ENDL;
//		LOG_INFO<< "edgetime (in ns) " << edgetime << ENDL;
//		LOG_INFO<< "With offset " << fabs(edgetime + LAVOffsetFinetime - finetime) << ENDL;
//		LOG_INFO<< "Without offset " << fabs(edgetime - finetime) << ENDL;
		/**
		 * Process leading edges only
		 *
		 */
//		if (edge_IDs[iEdge]
//				&& fabs(edgetime + LAVOffsetFinetime - finetime) <= 10.) {
		if (edge_IDs[iEdge]) {
			const int roChID = (edge_tdcIDs[iEdge] * 32) + edge_chIDs[iEdge];
//			LOG_INFO<< "Readout Channel ID " << roChID << ENDL;
//			LOG_INFO<< "Geom LeadGlass ID " << lgGeo[roChID] << ENDL;

			/**
			 * Process only first 128 readout channels, corresponding to low-threshold LAV FEE
			 *
			 */
//			if (lgGeo[roChID]) {
			if (roChID) {

//				LOG_INFO<< "Edge " << iEdge << " ID " << edge_IDs[iEdge] << ENDL;
//				LOG_INFO<< "Edge " << iEdge << " chID " << (uint) edge_chIDs[iEdge] << ENDL;
//				LOG_INFO<< "Edge " << iEdge << " tdcID " << (uint) edge_tdcIDs[iEdge] << ENDL;
//				LOG_INFO<< "Edge " << iEdge << " time " << std::hex << edge_times[iEdge] << std::dec << ENDL;
				nHits++;
			}
		}
	}
//	LOG_INFO<< "time check " << time[2].tv_sec << " " << time[2].tv_usec << ENDL;
//	}
	LOG_INFO<<" LAVAlgo.cpp: Analysed Event " << decoder.getDecodedEvent()->getEventNumber() << " - nHits " << nHits << ENDL;

//	LOG_INFO<< "time check (final)" << time[3].tv_sec << " " << time[3].tv_usec << ENDL;

//	for (int i = 0; i < 3; i++) {
//		if (i)
//			LOG_INFO<<((time[i+1].tv_sec - time[i].tv_sec)*1e6 + time[i+1].tv_usec) - time[i].tv_usec << " ";
//		}
//	LOG_INFO<< ((time[3].tv_sec - time[0].tv_sec)*1e6 + time[3].tv_usec) - time[0].tv_usec << ENDL;

	return (nHits < 2);

}

}
/* namespace na62 */

