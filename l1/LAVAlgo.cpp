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
#include "L1TriggerProcessor.h"

namespace na62 {

uint LAVAlgo::AlgoID_; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
uint LAVAlgo::AlgoLogic_[16];
uint LAVAlgo::AlgoRefTimeSourceID_[16];
double LAVAlgo::AlgoOnlineTimeWindow_[16];

LAVParsConfFile* LAVAlgo::InfoLAV_ = LAVParsConfFile::GetInstance();
int * LAVAlgo::LgGeo_ = InfoLAV_->getGeoLGMap();

void LAVAlgo::initialize(uint i, l1LAV &l1LAVStruct) {

	AlgoID_ = l1LAVStruct.configParams.l1TrigMaskID;
	AlgoLogic_[i] = l1LAVStruct.configParams.l1TrigLogic;
	AlgoRefTimeSourceID_[i] = l1LAVStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	AlgoOnlineTimeWindow_[i] = l1LAVStruct.configParams.l1TrigOnlineTimeWindow;
//	LOG_INFO("LAV mask: " << i << " logic " << AlgoLogic_[i] << " refTimeSourceID " << AlgoRefTimeSourceID_[i] << " online time window " << AlgoOnlineTimeWindow_[i]);
}

uint_fast8_t LAVAlgo::processLAVTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info) {

	using namespace l0;
//	LOG_INFO("Initial Time " << time[0].tv_sec << " " << time[0].tv_usec);
//	LOG_INFO("LAVAlgo: event timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

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
			LOG_ERROR("LAVAlgo.cpp: Not able to use averageCHODHitTime as Reference Time even if it is requested!");
	}
//	LOG_INFO("LAVAlgo: chodtime " << averageCHODHitTime);

	uint nHits = 0;
	uint nEdges_tot = 0;

	//TODO: chkmax need to be USED
	DecoderRange<TrbFragmentDecoder> x = decoder.getLAVDecoderRange();
	if (x.begin() == x.end()) {
		LOG_ERROR("LAV: Empty decoder range!");
		l1Info->setL1LAVBadData();
//		badData = 1;
		return 0;
	}

	int hit[maxNROchs] = { 0 };

	for (TrbFragmentDecoder* lavPacket : decoder.getLAVDecoderRange()) {
//	LOG_INFO("First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec);

		if (!lavPacket->isReady() || lavPacket->isBadFragment()) {
			LOG_ERROR("LAVAlgo: This looks like a Bad Packet!!!! ");
			l1Info->setL1LAVBadData();
//			badData = 1;
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

//		LOG_INFO("LAV: Tel62 ID " << lavPacket->getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard);
//		LOG_INFO("Reference detector fine time " << (uint) decoder.getDecodedEvent()->getFinetime());

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//			LOG_INFO("Edge " << iEdge << " ID " << edge_IDs[iEdge]);
//			LOG_INFO("Edge " << iEdge << " chID " << (uint) edge_chIDs[iEdge]);
//			LOG_INFO("Edge " << iEdge << " tdcID " << (uint) edge_tdcIDs[iEdge]);
//			LOG_INFO("Edge " << iEdge << " time " << std::hex << edge_times[iEdge] << std::dec);

//			if (edge_IDs[iEdge]) {
			const int roChID = edge_trbIDs * 512 + edge_tdcIDs[iEdge] * 32 + edge_chIDs[iEdge];

			const int roChIDPerTrb = edge_tdcIDs[iEdge] * 32 + edge_chIDs[iEdge];
//			LOG_INFO("LAV Algo: Found R0chID " << roChIDPerTrb);

			if ((roChID & 1) == 0) {
				if (edge_IDs[iEdge]) {
					edgetime = (edge_times[iEdge] - decoder.getDecodedEvent()->getTimestamp() * 256.) * 0.097464731802;

					if (!isCHODRefTime) {
						finetime = decoder.getDecodedEvent()->getFinetime() * 0.097464731802;
						dt_l0tp = fabs(edgetime - finetime);
						dt_chod = -1.0e+28;
					} else
						dt_chod = fabs(edgetime - averageCHODHitTime);

//					LOG_INFO("edgetime (in ns) " << edgetime);
//					LOG_INFO("finetime (in ns) " << finetime);
//					LOG_INFO("isLeading? " << edge_IDs[iEdge]);
//					LOG_INFO("Is Low Threshold:  " << ((roChID & 1) == 0) << " dt_l0tp " << dt_l0tp << " dt_chod " << dt_chod);

					if ((!isCHODRefTime && dt_l0tp < AlgoOnlineTimeWindow_[l0MaskID])
							|| (isCHODRefTime && dt_chod < AlgoOnlineTimeWindow_[l0MaskID])) {
						hit[roChIDPerTrb]++;
//						LOG_INFO("Increment hit[" << roChIDPerTrb << "] to " << hit[roChIDPerTrb]);
					}
				} else if (hit[roChIDPerTrb]) {
					nHits++;
//					LOG_INFO("Increment nHits " << nHits);

					if (nHits >= 3) {
						l1Info->setL1LAVNHits(nHits);
						l1Info->setL1LAVProcessed();
						return 1;
					}
				}
			} else {
//				printf("ODD! - Soglia alta! \n");
			}
		}
//	LOG_INFO("time check " << time[2].tv_sec << " " << time[2].tv_usec);
		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}

	if (!nEdges_tot)
		l1Info->setL1LAVEmptyPacket();
//	LOG_INFO("LAVAlgo.cpp: Analysed Event " << decoder.getDecodedEvent()->getEventNumber());
//	LOG_INFO("nEdges_tot " << nEdges_tot << " - nHits " << nHits);
//	LOG_INFO("time check (final)" << time[3].tv_sec << " " << time[3].tv_usec);

//	for (int i = 0; i < 3; i++) {
//		if (i) LOG_INFO(((time[i+1].tv_sec - time[i].tv_sec)*1e6 + time[i+1].tv_usec) - time[i].tv_usec << " ");
//		}
//	LOG_INFO(((time[3].tv_sec - time[0].tv_sec)*1e6 + time[3].tv_usec) - time[0].tv_usec);

	l1Info->setL1LAVNHits(nHits);
	l1Info->setL1LAVProcessed();
	return (nHits >= 3);
}

void LAVAlgo::writeData(L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {

	if (AlgoID_ != algoPacket->algoID)
		LOG_ERROR("Algo ID does not match with Algo ID written within the packet!");
	algoPacket->algoID = AlgoID_;
	algoPacket->onlineTimeWindow = (uint) AlgoOnlineTimeWindow_[l0MaskID];
//	algoPacket->qualityFlags = (l1Info->isL1LAVProcessed() << 6) | (l1Info->isL1LAVEmptyPacket() << 4) | (l1Info->isL1LAVBadData() << 2) | AlgoRefTimeSourceID_[l0MaskID];
	algoPacket->qualityFlags = (l1Info->isL1LAVProcessed() << 6) | (l1Info->isL1LAVEmptyPacket() << 4) | (l1Info->isL1LAVBadData() << 2)
			| ((uint) l1Info->getL1LAVTrgWrd());
	algoPacket->l1Data[0] = l1Info->getL1LAVNHits();
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
