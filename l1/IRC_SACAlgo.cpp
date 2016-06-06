/*
 * IRC_SACAlgo.cpp
 *
 *  Created on: 20 May 2016
 *      Author: lorenzaiacobuzio
 */
#include "IRC_SACAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#include <options/Logging.h>

#include "../common/decoding/TrbFragmentDecoder.h"
#include "ircsac_algorithm/ParsConfFile.h"

namespace na62 {

uint IRC_SACAlgo::algoID; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for MUV3, 5 for Straw
uint IRC_SACAlgo::algoLogic;
uint IRC_SACAlgo::algoRefTimeSourceID;
double IRC_SACAlgo::algoOnlineTimeWindow;

bool IRC_SACAlgo::algoProcessed = 0;
bool IRC_SACAlgo::emptyPacket = 0;
bool IRC_SACAlgo::badData = 0;
bool IRC_SACAlgo::isCHODRefTime = 0;

IRCSACParsConfFile* IRC_SACAlgo::infoIRC_SAC_ = IRCSACParsConfFile::GetInstance();
int * IRC_SACAlgo::lgGeo = infoIRC_SAC_->getGeoLGMap();
int IRC_SACAlgo::hit[maxNROchs];
uint IRC_SACAlgo::nHits;
double IRC_SACAlgo::averageCHODHitTime = 0.;
uint_fast8_t IRC_SACAlgo::numberOfEnabledL0Masks = 0;

IRC_SACAlgo::IRC_SACAlgo() {
}

IRC_SACAlgo::~IRC_SACAlgo() {
// TODO Auto-generated destructor stub
}

void IRC_SACAlgo::initialize(l1IRCSAC &l1IRCSACStruct, uint_fast8_t nEnabledMasks) {

	algoID = l1IRCSACStruct.configParams.l1TrigMaskID;
	algoLogic = l1IRCSACStruct.configParams.l1TrigLogic;
	algoRefTimeSourceID = l1IRCSACStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	algoOnlineTimeWindow = l1IRCSACStruct.configParams.l1TrigOnlineTimeWindow;
	numberOfEnabledL0Masks = nEnabledMasks;
}

uint_fast8_t IRC_SACAlgo::processIRCSACTrigger(DecoderHandler& decoder,
		L1InfoToStorage* l1Info) {

	using namespace l0;
//	LOG_INFO("Initial Time " << time[0].tv_sec << " " << time[0].tv_usec);
//	LOG_INFO("IRC_SACAlgo: event timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

	/*
	 * TODO: The same logic needs to be developed for RICHRefTime
	 */
	if (algoRefTimeSourceID == 1) {
		if (l1Info->isL1CHODProcessed() && averageCHODHitTime != -1.0e+28) {
			isCHODRefTime = 1;
			averageCHODHitTime = l1Info->getCHODAverageTime();
		} else LOG_ERROR("IRC_SACAlgo.cpp: Not able to use averageCHODHitTime as Reference Time even if it is requested!");
	}
//	LOG_INFO("IRC_SACAlgo: chodtime " << averageCHODHitTime);

	nHits = 0;

	//TODO: chkmax need to be USED
	DecoderRange<TrbFragmentDecoder> x = decoder.getIRCDecoderRange();
	if (x.begin() == x.end()) {
		LOG_ERROR("IRC_SAC: Empty decoder range!");
		badData = 1;
		return 0;
	}
	TrbFragmentDecoder& ircsacPacket =
				(TrbFragmentDecoder&) decoder.getDecodedIRCFragment(0);
//	LOG_INFO("First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec);

	 if (!ircsacPacket.isReady() || ircsacPacket.isBadFragment()) {

			LOG_ERROR("IRC_SACAlgo: This looks like a Bad Packet!!!! ");
			badData = 1;
			return 0;
		}

		for (uint i = 0; i != maxNROchs; i++) {
			hit[i] = 0;
		}

		/**
		 * Get Arrays with hit Info
		 */
		const uint64_t* const edge_times = ircsacPacket.getTimes();
		const uint_fast8_t* const edge_chIDs = ircsacPacket.getChIDs();
		const bool* const edge_IDs = ircsacPacket.getIsLeadings();
		const uint_fast8_t* const edge_tdcIDs = ircsacPacket.getTdcIDs();
		const uint_fast16_t edge_trbIDs = ircsacPacket.getFragmentNumber();
		double finetime, edgetime, dt_l0tp, dt_chod;

		uint numberOfEdgesOfCurrentBoard = ircsacPacket.getNumberOfEdgesStored();

//		LOG_INFO("IRC_SAC: Tel62 ID " << ircsacPacket->getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard);
//		LOG_INFO("Reference detector fine time " << decoder.getDecodedEvent()->getFinetime());

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//			LOG_INFO("Edge " << iEdge << " ID " << edge_IDs[iEdge]);
//			LOG_INFO("Edge " << iEdge << " chID " << (uint) edge_chIDs[iEdge]);
//			LOG_INFO("Edge " << iEdge << " tdcID " << (uint) edge_tdcIDs[iEdge]);
//			LOG_INFO("Edge " << iEdge << " time " << std::hex << edge_times[iEdge] << std::dec);

//			if (edge_IDs[iEdge]) {
			const int roChID = (edge_tdcIDs[iEdge] * 32) + edge_chIDs[iEdge];

			 if((roChID < 128) && ((roChID & 1) == 0)) {
				edgetime = (edge_times[iEdge]
						- decoder.getDecodedEvent()->getTimestamp() * 256.)
						* 0.097464731802;

				if (!isCHODRefTime) {
					finetime = decoder.getDecodedEvent()->getFinetime()
							* 0.097464731802;
					dt_l0tp = fabs(edgetime - finetime);
					dt_chod = -1.0e+28;
				} else
					dt_chod = fabs(edgetime - averageCHODHitTime);

//				LOG_INFO("edgetime (in ns) " << edgetime);
//				LOG_INFO("finetime (in ns) " << finetime);
//				LOG_INFO("dt_l0tp " << dt_l0tp << " dt_chod " << dt_chod);

//				if (dt_l0tp < 20.) {
				if ((!isCHODRefTime && dt_l0tp < algoOnlineTimeWindow)
						|| (isCHODRefTime && dt_chod < algoOnlineTimeWindow)) {
					if (edge_IDs[iEdge]) {
						hit[roChID]++;

//						LOG_INFO("Increment hit[" << roChIDPerTrb << "] to " << hit[roChIDPerTrb]);
					} else if (hit[roChID]) {
						nHits++;
//						LOG_INFO("Increment nHits " << nHits);
						if ((nHits >= 3 && !algoLogic)
								|| (nHits < 3 && algoLogic)) {
							algoProcessed = 1;
							return 0;
						}
					}
				}
			} else {
//				printf("ODD! - Soglia alta! - Out Of Time! \n");
			}
		}
//	LOG_INFO("time check " << time[2].tv_sec << " " << time[2].tv_usec);

	if (!numberOfEdgesOfCurrentBoard) emptyPacket = 1;
//	LOG_INFO("IRC_SACAlgo.cpp: Analysed Event " << decoder.getDecodedEvent()->getEventNumber() << " - nEdges_tot " << nEdges_tot << " - nHits " << nHits);
//	LOG_INFO("time check (final)" << time[3].tv_sec << " " << time[3].tv_usec);

//	for (int i = 0; i < 3; i++) {
//		if (i)
//			LOG_INFO(((time[i+1].tv_sec - time[i].tv_sec)*1e6 + time[i+1].tv_usec) - time[i].tv_usec << " ");
//		}
//	LOG_INFO(((time[3].tv_sec - time[0].tv_sec)*1e6 + time[3].tv_usec) - time[0].tv_usec);

//	return (nHits < 3);
	algoProcessed = 1;
	if (!algoLogic)
		return (nHits < 3);
	else
		return (nHits >= 3);
}

bool IRC_SACAlgo::isAlgoProcessed() {
	return algoProcessed;
}

void IRC_SACAlgo::resetAlgoProcessed() {
	algoProcessed = 0;
}

bool IRC_SACAlgo::isEmptyPacket() {
	return emptyPacket;
}

bool IRC_SACAlgo::isBadData() {
	return badData;
}

void IRC_SACAlgo::writeData(L1Block &l1Block){

	for(int iMask=0; iMask<numberOfEnabledL0Masks; iMask++){
	  (l1Block.l1Mask[iMask]).l1Algo[algoID].l1AlgoID = algoID;
	  (l1Block.l1Mask[iMask]).l1Algo[algoID].l1AlgoProcessed = algoProcessed;
	  (l1Block.l1Mask[iMask]).l1Algo[algoID].l1AlgoOnlineTimeWindow = algoOnlineTimeWindow;
	}
}

}
/* namespace na62 */

