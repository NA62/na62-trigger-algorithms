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

uint IRC_SACAlgo::AlgoID_; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
uint IRC_SACAlgo::AlgoLogic_[16];
uint IRC_SACAlgo::AlgoRefTimeSourceID_[16];
double IRC_SACAlgo::AlgoOnlineTimeWindow_[16];

IRCSACParsConfFile* IRC_SACAlgo::InfoIRCSAC_ = IRCSACParsConfFile::GetInstance();
int * IRC_SACAlgo::LgGeo_ = InfoIRCSAC_->getGeoLGMap();

void IRC_SACAlgo::initialize(uint i, l1IRCSAC &l1IRCSACStruct) {

	AlgoID_ = l1IRCSACStruct.configParams.l1TrigMaskID;
	AlgoLogic_[i] = l1IRCSACStruct.configParams.l1TrigLogic;
	AlgoRefTimeSourceID_[i] = l1IRCSACStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	AlgoOnlineTimeWindow_[i] = l1IRCSACStruct.configParams.l1TrigOnlineTimeWindow;
//	LOG_INFO("IRCSAC mask: " << i << " logic " << AlgoLogic_[i] << " refTimeSourceID " << AlgoRefTimeSourceID_[i] << " online time window " << AlgoOnlineTimeWindow_[i]);
}

uint_fast8_t IRC_SACAlgo::processIRCSACTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info) {

	using namespace l0;
//	LOG_INFO("Initial Time " << time[0].tv_sec << " " << time[0].tv_usec);
//	LOG_INFO("IRC_SACAlgo: event timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

	/*
	 * TODO: The same logic needs to be developed for RICHRefTime
	 */
	double averageCHODHitTime = 0.;
	bool isCHODRefTime = false;
	if (AlgoRefTimeSourceID_[l0MaskID] == 1) {
		if (l1Info->isL1CHODProcessed()) {
			isCHODRefTime = 1;
			averageCHODHitTime = l1Info->getCHODAverageTime();
		} else
			LOG_ERROR("IRC_SACAlgo.cpp: Not able to use averageCHODHitTime as Reference Time even if it is requested!");
	}
//	LOG_INFO("IRC_SACAlgo: chodtime " << averageCHODHitTime);

	uint nHits = 0;

	//TODO: chkmax need to be USED
	DecoderRange<TrbFragmentDecoder> x = decoder.getIRCDecoderRange();
	if (x.begin() == x.end()) {
		LOG_ERROR("IRC_SAC: Empty decoder range!");
		l1Info->setL1IRCSACBadData();
//		badData = 1;
		return 0;
	}

	int hit[maxNROchs] = { 0 };

	TrbFragmentDecoder& ircsacPacket = (TrbFragmentDecoder&) decoder.getDecodedIRCFragment(0);
//	LOG_INFO("First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec);

	if (!ircsacPacket.isReady() || ircsacPacket.isBadFragment()) {
		LOG_ERROR("IRC_SACAlgo: This looks like a Bad Packet!!!! ");
		l1Info->setL1IRCSACBadData();
//		badData = 1;
		return 0;
	}

	/**
	 * Get Arrays with hit Info
	 */
	const uint64_t* const edge_times = ircsacPacket.getTimes();
	const uint_fast8_t* const edge_chIDs = ircsacPacket.getChIDs();
	const bool* const edge_IDs = ircsacPacket.getIsLeadings();
	const uint_fast8_t* const edge_tdcIDs = ircsacPacket.getTdcIDs();
	double finetime, edgetime, dt_l0tp, dt_chod;

	uint numberOfEdgesOfCurrentBoard = ircsacPacket.getNumberOfEdgesStored();

//	LOG_INFO("IRC_SAC: Tel62 ID " << ircsacPacket.getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard);
//	LOG_INFO("Reference detector fine time " << (uint) decoder.getDecodedEvent()->getFinetime());

	for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//		LOG_INFO("Edge " << iEdge << " ID " << edge_IDs[iEdge]);
//		LOG_INFO("Edge " << iEdge << " chID " << (uint) edge_chIDs[iEdge]);
//		LOG_INFO("Edge " << iEdge << " tdcID " << (uint) edge_tdcIDs[iEdge]);
//		LOG_INFO("Edge " << iEdge << " time " << std::hex << edge_times[iEdge] << std::dec);

		const int roChID = (edge_tdcIDs[iEdge] * 32) + edge_chIDs[iEdge];
//		LOG_INFO("IRCSAC Algo: Found R0chID " << roChID);

		if ((roChID < 128) && ((roChID & 1) == 0)) {
			if (edge_IDs[iEdge]) {
				edgetime = (edge_times[iEdge] - decoder.getDecodedEvent()->getTimestamp() * 256.) * 0.097464731802;

				if (!isCHODRefTime) {
					finetime = decoder.getDecodedEvent()->getFinetime() * 0.097464731802;
					dt_l0tp = fabs(edgetime - finetime);
					dt_chod = -1.0e+28;
				} else
					dt_chod = fabs(edgetime - averageCHODHitTime);

//				LOG_INFO("edgetime (in ns) " << edgetime);
//				LOG_INFO("finetime (in ns) " << finetime);
//				LOG_INFO("isLeading? " << edge_IDs[iEdge]);
//				LOG_INFO("Is Low Threshold:  " << ((roChID & 1) == 0) << " dt_l0tp " << dt_l0tp << " dt_chod " << dt_chod);

				if ((!isCHODRefTime && dt_l0tp < AlgoOnlineTimeWindow_[l0MaskID])
						|| (isCHODRefTime && dt_chod < AlgoOnlineTimeWindow_[l0MaskID])) {
					hit[roChID]++;
//					LOG_INFO("Increment hit[" << roChID << "] to " << hit[roChID]);
				}
			} else if (hit[roChID]) {
				nHits++;
//				LOG_INFO("Increment nHits " << nHits);

				if (nHits) {
					l1Info->setL1IRCSACNHits(nHits);
					l1Info->setL1IRCSACProcessed();
					return 1;
				}
			}
		} else {
//			printf("ODD! - Soglia alta! \n");
		}
	}
//	LOG_INFO("time check " << time[2].tv_sec << " " << time[2].tv_usec);

	if (!numberOfEdgesOfCurrentBoard)
		l1Info->setL1IRCSACEmptyPacket();
//	LOG_INFO("IRC_SACAlgo.cpp: Analysed Event " << decoder.getDecodedEvent()->getEventNumber());
//	LOG_INFO("nEdges_tot " << numberOfEdgesOfCurrentBoard << " - nHits " << nHits);
//	LOG_INFO("time check (final)" << time[3].tv_sec << " " << time[3].tv_usec);

//	for (int i = 0; i < 3; i++) {
//		if (i) LOG_INFO(((time[i+1].tv_sec - time[i].tv_sec)*1e6 + time[i+1].tv_usec) - time[i].tv_usec << " ");
//		}
//	LOG_INFO(((time[3].tv_sec - time[0].tv_sec)*1e6 + time[3].tv_usec) - time[0].tv_usec);

	l1Info->setL1IRCSACNHits(nHits);
	l1Info->setL1IRCSACProcessed();
	return (nHits);
}

void IRC_SACAlgo::writeData(L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {

	if (AlgoID_ != algoPacket->algoID)
		LOG_ERROR("Algo ID does not match with Algo ID written within the packet!");
	algoPacket->algoID = AlgoID_;
	algoPacket->onlineTimeWindow = (uint) AlgoOnlineTimeWindow_[l0MaskID];
//	algoPacket->qualityFlags = (l1Info->isL1IRCSACProcessed() << 6) | (l1Info->isL1IRCSACEmptyPacket() << 4) | (l1Info->isL1IRCSACBadData() << 2) | AlgoRefTimeSourceID_[l0MaskID];
	algoPacket->qualityFlags = (l1Info->isL1IRCSACProcessed() << 6) | (l1Info->isL1IRCSACEmptyPacket() << 4)
			| (l1Info->isL1IRCSACBadData() << 2) | ((uint) l1Info->getL1IRCSACTrgWrd());
	algoPacket->l1Data[0] = l1Info->getL1IRCSACNHits();
	if (AlgoRefTimeSourceID_[l0MaskID] == 1)
		algoPacket->l1Data[1] = l1Info->getCHODAverageTime();
	else
		algoPacket->l1Data[1] = 0;
	algoPacket->numberOfWords = (sizeof(L1Algo) / 4.);
//	LOG_INFO("l0MaskID " << l0MaskID);
//	LOG_INFO("algoID " << (uint)algoPacket->algoID);
//	LOG_INFO("quality Flags " << (uint)algoPacket->qualityFlags);
//	LOG_INFO("online TW " << (uint)algoPacket->onlineTimeWindow);
//	LOG_INFO("Data Words " << algoPacket->l1Data[0] << " " << algoPacket->l1Data[1]);

}

}
/* namespace na62 */

