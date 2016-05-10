/*
 * MUVAlgo.cpp
 *
 *  Created on: 24 Mar 2016
 *      Author: lorenza
 */

#include "MUVAlgo.h"

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

MUV3ParsConfFile* MUV3Algo::infoMUV3_ = MUV3ParsConfFile::GetInstance();
int * MUV3Algo::pmtGeo = infoMUV3_->getGeoPMTMap();
//uint MUV3Algo::nHits;
//uint MUV3Algo::nMaxPMTs;
uint MUV3Algo::pmtID1;
uint MUV3Algo::pmtID2;
int MUV3Algo::roChID;

MUV3Algo::MUV3Algo() {
}

MUV3Algo::~MUV3Algo() {
// TODO Auto-generated destructor stub
}

uint_fast8_t MUV3Algo::processMUV3Trigger1(DecoderHandler& decoder) {

	using namespace l0;

	//nMaxPMTs = 0;
	//nHits = 0;

	//LOG_INFO("Event number = " << decoder.getDecodedEvent()->getEventNumber());
	//LOG_INFO("Timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

	TrbFragmentDecoder& muv3Packet =
			(TrbFragmentDecoder&) decoder.getDecodedMUV3Fragment(0);
	/**
	 * Get Arrays with hit Info
	 */

	//const uint64_t* const edge_times = muv3Packet.getTimes();
	const uint_fast8_t* const edge_chIDs = muv3Packet.getChIDs();
	const bool* const edge_IDs = muv3Packet.getIsLeadings();
	const uint_fast8_t* const edge_tdcIDs = muv3Packet.getTdcIDs();
	//double finetime, edgetime;

	uint numberOfEdgesOfCurrentBoard = muv3Packet.getNumberOfEdgesStored();
//	LOG_INFO("MUV3: Tel62 ID " << muv3Packet.getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard);

	for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {

		/*edgetime = (edge_times[iEdge]
				- decoder.getDecodedEvent()->getTimestamp() * 256.)
				* 0.097464731802;*/

//		LOG_INFO<< "finetime (decoder) " << (uint)decoder.getDecodedEvent()->getFinetime() << ENDL;
//		LOG_INFO<< "edge_time " << std::hex << edge_times[iEdge] << std::dec << ENDL;
//		LOG_INFO<< "finetime (in ns) " << finetime << ENDL;
//		LOG_INFO<< "edgetime (in ns) " << edgetime << ENDL;
//		LOG_INFO<< "Without offset " << fabs(edgetime - finetime) << ENDL;

		/**
		 * Process leading edges only
		 *
		 */
		if (edge_IDs[iEdge]) {
			roChID = (edge_tdcIDs[iEdge] * 32) + edge_chIDs[iEdge];
			if (pmtGeo[roChID] > 151) pmtID1 = pmtGeo[roChID] - 200;
			else pmtID1 = pmtGeo[roChID];
			for (uint jEdge = 0; jEdge != numberOfEdgesOfCurrentBoard; jEdge++) {
				if (edge_IDs[jEdge] && jEdge != iEdge) {
					roChID = (edge_tdcIDs[jEdge] * 32) + edge_chIDs[jEdge];
					if (pmtGeo[roChID] > 151) pmtID2 = pmtGeo[roChID] - 200;
					else pmtID2 = pmtGeo[roChID];
					if (((pmtID1 % 12) <= 5) && ((pmtID2 % 12) >= 6) && pmtID1 < 144 && pmtID2 < 144) {
							//LOG_INFO("left and right! pmt1= " << pmtID1 << " pmt2= " << pmtID2);
						return 1;}
					else if (((pmtID2 % 12) <= 5) && ((pmtID1 % 12) >= 6) && pmtID1 < 144 && pmtID2 < 144) {
							//LOG_INFO("left and right! pmt1= " << pmtID1 << " pmt2= " << pmtID2);
						return 1;}
				}
			}
		}
	}
}

uint_fast8_t MUV3Algo::processMUV3Trigger2(DecoderHandler& decoder) {

	using namespace l0;

	//LOG_INFO("Event number = " << decoder.getDecodedEvent()->getEventNumber());

	TrbFragmentDecoder& muv3Packet =
			(TrbFragmentDecoder&) decoder.getDecodedMUV3Fragment(0);

	//const uint64_t* const edge_times = muv3Packet.getTimes();
	const uint_fast8_t* const edge_chIDs = muv3Packet.getChIDs();
	const bool* const edge_IDs = muv3Packet.getIsLeadings();
	const uint_fast8_t* const edge_tdcIDs = muv3Packet.getTdcIDs();
	//double finetime, edgetime;

	uint numberOfEdgesOfCurrentBoard = muv3Packet.getNumberOfEdgesStored();
	//LOG_INFO<< "MUV3: Tel62 ID " << muv3Packet.getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard << ENDL;

	for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
		if (edge_IDs[iEdge]) {
			roChID = (edge_tdcIDs[iEdge] * 32) + edge_chIDs[iEdge];
			if (pmtGeo[roChID] > 151) pmtID1 = pmtGeo[roChID] - 200;
			else pmtID1 = pmtGeo[roChID];
			for (uint jEdge = 0; jEdge != numberOfEdgesOfCurrentBoard; jEdge++) {
				if (edge_IDs[jEdge] && jEdge != iEdge) {
					roChID = (edge_tdcIDs[jEdge] * 32) + edge_chIDs[jEdge];
					if (pmtGeo[roChID] > 151) pmtID2 = pmtGeo[roChID] - 200;
					else pmtID2 = pmtGeo[roChID];
					if (abs(pmtID1-pmtID2) == 1 && ((pmtID1+pmtID2) % 24) != 23 && pmtID1 < 144 && pmtID2 < 144) {
//							LOG_INFO("neighbours! pmt1= " << pmtID1 << " pmt2= " << pmtID2);
						return 1;}
					else if (abs(pmtID1-pmtID2) == 12 && pmtID1 < 144 && pmtID2 < 144) {
//							LOG_INFO("neighbours! pmt1= " << pmtID1 << " pmt2= " << pmtID2);
						return 1;}
				}
			}
		}
	}
}
}
/* namespace na62 */

