/*
 * LAVAlgo.cpp
 *
 *  Created on: 7 Sep 2015
 *      Author: romano
 */
#include "LAVAlgo.h"

#include <string.h>
#include <math.h>
#include <sys/time.h>

#ifndef ONLINEHLT
#include <l0/Subevent.h>
#else
#include <l0/offline/Subevent.h>
#endif
#include <options/Logging.h>

#include <common/decoding/TrbFragmentDecoder.h>

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
//	LOG_INFO("LAV: event timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);
//	LOG_INFO("LAV: event reference fine time from L0TP " << std::hex << (uint)decoder.getDecodedEvent()->getFinetime() << std::dec);

	/*
	 * TODO: The same logic needs to be developed for RICHRefTime
	 */
	double refTimeL0TP = 0.;
	double averageCHODHitTime = 0.;
	bool isCHODRefTime = false;
	if (!AlgoRefTimeSourceID_[l0MaskID]) {
		refTimeL0TP = decoder.getDecodedEvent()->getFinetime() * 0.097464731802;
//		LOG_INFO("L1 reference finetime from L0TP (ns) " << refTimeL0TP);
	} else if (AlgoRefTimeSourceID_[l0MaskID] == 1) {
		if (l1Info->isL1CHODProcessed()) {
			isCHODRefTime = 1;
			averageCHODHitTime = l1Info->getCHODAverageTime();
		} else
			LOG_ERROR("LAVAlgo.cpp: Not able to use averageCHODHitTime as Reference Time even if requested!");
	} else
		LOG_ERROR("L1 Reference Time Source ID not recognised !!");

	uint nHits = 0;
	uint nEdgesTot = 0;

	//TODO: chkmax need to be USED
	DecoderRange<TrbFragmentDecoder> x = decoder.getLAVDecoderRange();
	if (x.begin() == x.end()) {
		LOG_ERROR("LAV: Empty decoder range!");
		l1Info->setL1LAVBadData();
		return 0;
	}

	int hit[maxNROchs] = { 0 };

	for (TrbFragmentDecoder* lavPacket : decoder.getLAVDecoderRange()) {
//	LOG_INFO("First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec);

		if(lavPacket->getSourceSubId() == 0) continue;

		if (!lavPacket->isReady() || lavPacket->isBadFragment()) {
			LOG_ERROR("LAV: This looks like a Bad Packet!!!! ");
			l1Info->setL1LAVBadData();
			return 0;
		}

		for (uint i = 0; i != maxNROchs; i++) {
			hit[i] = 0;
		}

		/**
		 * Get Arrays with hit Info
		 */
		const uint64_t* const edgeTime = lavPacket->getTimes();
		const uint_fast8_t* const edgeChID = lavPacket->getChIDs();
		const bool* const edgeIsLeading = lavPacket->getIsLeadings();
		const uint_fast8_t* const edgeTdcID = lavPacket->getTdcIDs();
		const uint_fast16_t edgeTrbID = lavPacket->getFragmentNumber();
		double time, dtL0TP, dtCHOD;

		uint numberOfEdgesOfCurrentBoard = lavPacket->getNumberOfEdgesStored();

//		LOG_INFO("LAV: Fragment ID " << lavPacket->getFragmentNumber() << " Tel62 ID " << lavPacket->getSourceSubId() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard);

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//			LOG_INFO("Edge " << iEdge << " ID " << edgeIsLeading[iEdge]);
//			LOG_INFO("Edge " << iEdge << " chID " << (uint) edgeChID[iEdge]);
//			LOG_INFO("Edge " << iEdge << " tdcID " << (uint) edgeTdcID[iEdge]);
//			LOG_INFO("Edge " << iEdge << " time " << std::hex << edgeTime[iEdge] << std::dec);

			const int roChID = edgeTrbID * 512 + edgeTdcID[iEdge] * 32 + edgeChID[iEdge];
			const int roChIDPerTrb = edgeTdcID[iEdge] * 32 + edgeChID[iEdge];
//			LOG_INFO("LAV Algo: Found R0chID " << roChIDPerTrb);

			if ((roChID & 1) == 0) {
				if (edgeIsLeading[iEdge]) {
					time = (edgeTime[iEdge] - decoder.getDecodedEvent()->getTimestamp() * 256.) * 0.097464731802;
//					LOG_INFO("Edge time (ns) " << time);

					if (!isCHODRefTime) {
						dtL0TP = fabs(time - refTimeL0TP);
						dtCHOD = -1.0e+28;
					} else
						dtCHOD = fabs(time - averageCHODHitTime);
//					LOG_INFO("Is Low Threshold:  " << ((roChID & 1) == 0) << " dtL0TP " << dtL0TP << " dtCHOD " << dtCHOD);

					if ((!isCHODRefTime && dtL0TP < AlgoOnlineTimeWindow_[l0MaskID])
							|| (isCHODRefTime && dtCHOD < AlgoOnlineTimeWindow_[l0MaskID])) {
						hit[roChIDPerTrb]++;
//						LOG_INFO("Increment hit[" << roChIDPerTrb << "] to " << hit[roChIDPerTrb]);
					}
				} else if (hit[roChIDPerTrb]) {
					nHits++;
//					LOG_INFO("Increment nHits " << nHits);

					if (nHits >= 3) {
						l1Info->setL1LAVNHits(nHits);
						l1Info->setL1LAVProcessed();
//						LOG_INFO("LAVAlgo: Returning nHits >= 3 !!!!");
						return 1;
					}
				}
			} else {
//				printf("ODD! - Soglia alta! \n");
			}
		}
//	LOG_INFO("time check " << time[2].tv_sec << " " << time[2].tv_usec);
		nEdgesTot += numberOfEdgesOfCurrentBoard;
	}

	if (!nEdgesTot)
		l1Info->setL1LAVEmptyPacket();

//	LOG_INFO("LAV: Analysing Event " << decoder.getDecodedEvent()->getEventNumber());
//	LOG_INFO("Timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);
//	LOG_INFO("time check (final)" << time[3].tv_sec << " " << time[3].tv_usec);

//	for (int i = 0; i < 3; i++) {
//		if (i)
//			LOG_INFO(((time[i+1].tv_sec - time[i].tv_sec)*1e6 + time[i+1].tv_usec) - time[i].tv_usec << " ");
//	}
//	LOG_INFO(((time[3].tv_sec - time[0].tv_sec)*1e6 + time[3].tv_usec) - time[0].tv_usec);

//	LOG_INFO("nEdges_tot " << nEdgesTot << " - nHits " << nHits);

	l1Info->setL1LAVNHits(nHits);
	l1Info->setL1LAVProcessed();
	return (nHits >= 3);
}

void LAVAlgo::writeData(L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {

	if (AlgoID_ != algoPacket->algoID)
		LOG_ERROR("Algo ID does not match with Algo ID already written within the packet!");

	algoPacket->algoID = AlgoID_;
	algoPacket->onlineTimeWindow = (uint) AlgoOnlineTimeWindow_[l0MaskID];
//	algoPacket->qualityFlags = (l1Info->isL1LAVProcessed() << 6) | (l1Info->isL1LAVEmptyPacket() << 4) | (l1Info->isL1LAVBadData() << 2) | AlgoRefTimeSourceID_[l0MaskID];
	algoPacket->qualityFlags = (l1Info->isL1LAVProcessed() << 6) | (l1Info->isL1LAVEmptyPacket() << 4) | (l1Info->isL1LAVBadData() << 2)
			| ((uint) l1Info->getL1LAVTrgWrd(l0MaskID));

	algoPacket->l1Data[0] = l1Info->getL1LAVNHits();
	if (!AlgoRefTimeSourceID_[l0MaskID]) {
		algoPacket->l1Data[1] = l1Info->getL1RefTimeL0TP();
	} else if (AlgoRefTimeSourceID_[l0MaskID] == 1) {
		algoPacket->l1Data[1] = l1Info->getCHODAverageTime(); //this is a double!!!
	} else
		LOG_ERROR("L1 Reference Time Source ID not recognised !!");

	algoPacket->numberOfWords = (sizeof(L1Algo) / 4.);
//	LOG_INFO("l0MaskID " << l0MaskID);
//	LOG_INFO("algoID " << (uint)algoPacket->algoID);
//	LOG_INFO("quality Flags " << (uint)algoPacket->qualityFlags);
//	LOG_INFO("online TW " << (uint)algoPacket->onlineTimeWindow);
//	LOG_INFO("Data Words " << algoPacket->l1Data[0] << " " << algoPacket->l1Data[1]);
}

}
/* namespace na62 */
