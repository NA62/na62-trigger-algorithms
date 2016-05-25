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

uint MUV3Algo::algoID; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for MUV3, 5 for Straw
uint MUV3Algo::algoLogic;
uint MUV3Algo::algoRefTimeSourceID;
double MUV3Algo::algoOnlineTimeWindow;

bool MUV3Algo::algoProcessed = 0;
bool MUV3Algo::emptyPacket = 0;
bool MUV3Algo::badData = 0;
bool MUV3Algo::isCHODRefTime = 0;
double MUV3Algo::averageCHODHitTime = 0.;

MUV3ParsConfFile* MUV3Algo::infoMUV3_ = MUV3ParsConfFile::GetInstance();
int * MUV3Algo::pmtGeo = infoMUV3_->getGeoPMTMap();
int MUV3Algo::roChID;
uint MUV3Algo::pmtID1;
uint MUV3Algo::pmtID2;

MUV3Algo::MUV3Algo() {
}

MUV3Algo::~MUV3Algo() {
// TODO Auto-generated destructor stub
}

void MUV3Algo::initialize(l1MUV &l1MUV3Struct) {

	algoID = l1MUV3Struct.configParams.l1TrigMaskID;
	algoLogic = l1MUV3Struct.configParams.l1TrigLogic;
	algoRefTimeSourceID = l1MUV3Struct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	algoOnlineTimeWindow = l1MUV3Struct.configParams.l1TrigOnlineTimeWindow;

}

uint_fast8_t MUV3Algo::processMUV3Trigger1(DecoderHandler& decoder,
		L1InfoToStorage* l1Info) {

	using namespace l0;
	//LOG_INFO("Event number = " << decoder.getDecodedEvent()->getEventNumber());
	//LOG_INFO("Timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

	/*
	 * TODO: The same logic needs to be developed for RICHRefTime
	 */
	if (algoRefTimeSourceID == 1) {
		if (l1Info->isL1CHODProcessed() && averageCHODHitTime != -1.0e+28) {
			isCHODRefTime = 1;
			averageCHODHitTime = l1Info->getCHODAverageTime();
		} else
			LOG_ERROR(
					"MUV3Algo.cpp: Not able to use averageCHODHitTime as Reference Time even if it is requested!");
	}

	TrbFragmentDecoder& muv3Packet =
			(TrbFragmentDecoder&) decoder.getDecodedMUV3Fragment(0);
	if (!muv3Packet.isReady() || muv3Packet.isBadFragment()) {

		LOG_ERROR("MUV3Algo: This looks like a Bad Packet!!!! ");
		badData = 1;
		return 0;
	}

	/**
	 * Get Arrays with hit Info
	 */

	//const uint64_t* const edge_times = muv3Packet.getTimes();
	const uint_fast8_t* const edge_chIDs = muv3Packet.getChIDs();
	const bool* const edge_IDs = muv3Packet.getIsLeadings();
	const uint_fast8_t* const edge_tdcIDs = muv3Packet.getTdcIDs();
	//double finetime, edgetime;

	uint numberOfEdgesOfCurrentBoard = muv3Packet.getNumberOfEdgesStored();
	if (!numberOfEdgesOfCurrentBoard)
		emptyPacket = 1;
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
			if (pmtGeo[roChID] > 151)
				pmtID1 = pmtGeo[roChID] - 200;
			else
				pmtID1 = pmtGeo[roChID];
			for (uint jEdge = 0; jEdge != numberOfEdgesOfCurrentBoard;
					jEdge++) {
				if (edge_IDs[jEdge] && jEdge != iEdge) {
					roChID = (edge_tdcIDs[jEdge] * 32) + edge_chIDs[jEdge];
					if (pmtGeo[roChID] > 151)
						pmtID2 = pmtGeo[roChID] - 200;
					else
						pmtID2 = pmtGeo[roChID];

					if (pmtID1 < 144 && pmtID2 < 144) {
						if (((pmtID1 % 12) <= 5 && (pmtID2 % 12) >= 6)
								|| ((pmtID2 % 12) <= 5 && (pmtID1 % 12) >= 6)) {
							//LOG_INFO("left and right! pmt1= " << pmtID1 << " pmt2= " << pmtID2);
							algoProcessed = 1;
							if (algoLogic)
								return 1;
							else
								return 0;
						}
					}
				}
			}
		}
	}

	algoProcessed = 1;
	return 0;
}

uint_fast8_t MUV3Algo::processMUV3Trigger2(DecoderHandler& decoder,
		L1InfoToStorage* l1Info) {

	using namespace l0;
	//LOG_INFO("Event number = " << decoder.getDecodedEvent()->getEventNumber());
	//LOG_INFO("Timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

	/*
	 * TODO: The same logic needs to be developed for RICHRefTime
	 */
	if (algoRefTimeSourceID == 1) {
		if (l1Info->isL1CHODProcessed() && averageCHODHitTime != -1.0e+28) {
			isCHODRefTime = 1;
			averageCHODHitTime = l1Info->getCHODAverageTime();
		} else
			LOG_ERROR(
					"MUV3Algo.cpp: Not able to use averageCHODHitTime as Reference Time even if it is requested!");
	}

	TrbFragmentDecoder& muv3Packet =
			(TrbFragmentDecoder&) decoder.getDecodedMUV3Fragment(0);
	if (!muv3Packet.isReady() || muv3Packet.isBadFragment()) {

		LOG_ERROR("MUV3Algo: This looks like a Bad Packet!!!! ");
		badData = 1;
		return 0;
	}

	/**
	 * Get Arrays with hit Info
	 */
	//const uint64_t* const edge_times = muv3Packet.getTimes();
	const uint_fast8_t* const edge_chIDs = muv3Packet.getChIDs();
	const bool* const edge_IDs = muv3Packet.getIsLeadings();
	const uint_fast8_t* const edge_tdcIDs = muv3Packet.getTdcIDs();
	//double finetime, edgetime;

	uint numberOfEdgesOfCurrentBoard = muv3Packet.getNumberOfEdgesStored();
	if (!numberOfEdgesOfCurrentBoard)
		emptyPacket = 1;
	//LOG_INFO<< "MUV3: Tel62 ID " << muv3Packet.getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard << ENDL;

	for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
		if (edge_IDs[iEdge]) {
			roChID = (edge_tdcIDs[iEdge] * 32) + edge_chIDs[iEdge];
			if (pmtGeo[roChID] > 151)
				pmtID1 = pmtGeo[roChID] - 200;
			else
				pmtID1 = pmtGeo[roChID];
			for (uint jEdge = 0; jEdge != numberOfEdgesOfCurrentBoard;
					jEdge++) {
				if (edge_IDs[jEdge] && jEdge != iEdge) {
					roChID = (edge_tdcIDs[jEdge] * 32) + edge_chIDs[jEdge];
					if (pmtGeo[roChID] > 151)
						pmtID2 = pmtGeo[roChID] - 200;
					else
						pmtID2 = pmtGeo[roChID];

					if (pmtID1 < 144 && pmtID2 < 144) {
						if ((fabs(pmtID1 - pmtID2) == 1
								&& ((pmtID1 + pmtID2) % 24) != 23)
								|| fabs(pmtID1 - pmtID2) == 12) {
//							LOG_INFO("neighbours! pmt1= " << pmtID1 << " pmt2= " << pmtID2);
							algoProcessed = 1;
							if (algoLogic)
								return 1;
							else
								return 0;
						}
					}
				}
			}
		}
	}

	algoProcessed = 1;
	return 0;
}

}
/* namespace na62 */

