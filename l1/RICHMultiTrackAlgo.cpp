/*
 * RICHMultiTrackAlgo.cpp
 *
 *  Created on: 7 Aug 2017
 *      Author: romano
 */

#include "RICHMultiTrackAlgo.h"

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

uint RICHMultiTrackAlgo::AlgoID_; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
uint RICHMultiTrackAlgo::AlgoLogic_[16];
uint RICHMultiTrackAlgo::AlgoRefTimeSourceID_[16];
double RICHMultiTrackAlgo::AlgoOnlineTimeWindow_[16];

RICHParsConfFile* RICHMultiTrackAlgo::InfoRICH_ = RICHParsConfFile::GetInstance();
int * RICHMultiTrackAlgo::PMTGeo_ = InfoRICH_->getGeoPmsMap();


void RICHMultiTrackAlgo::initialize(uint i, l1RICH &l1RICHStruct) {

	AlgoID_ = l1RICHStruct.configParams.l1TrigMaskID;
	AlgoLogic_[i] = l1RICHStruct.configParams.l1TrigLogic;
	AlgoRefTimeSourceID_[i] = l1RICHStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	AlgoOnlineTimeWindow_[i] = l1RICHStruct.configParams.l1TrigOnlineTimeWindow;
//	LOG_INFO("RICH mask: " << i << " logic " << AlgoLogic_[i] << " refTimeSourceID " << AlgoRefTimeSourceID_[i] << " online time window " << AlgoOnlineTimeWindow_[i]);
}

uint_fast8_t RICHMultiTrackAlgo::processRICHMultiTrackTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info) {

	using namespace l0;

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
			LOG_ERROR("RICHMultiTrackAlgo.cpp: Not able to use averageCHODHitTime as Reference Time even if requested!");
	} else
		LOG_ERROR("L1 Reference Time Source ID not recognised !!");

	uint nHits = 0;
	uint nEdgesTot = 0;

	//TODO: chkmax need to be USED
	DecoderRange<TrbFragmentDecoder> x = decoder.getLAVDecoderRange();
	if (x.begin() == x.end()) {
		LOG_ERROR("RICH MultiTrack: Empty decoder range!");
		//l1Info->setL1RICHBadData();
		return 0;
	}

	int hit[maxNROchs] = { 0 };

	for (TrbFragmentDecoder* richPacket : decoder.getRICHDecoderRange()) {
//	LOG_INFO("First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec);

		if (!richPacket->isReady() || richPacket->isBadFragment()) {
			LOG_ERROR("RICH: This looks like a Bad Packet!!!! ");
			//l1Info->setL1RICHBadData();
			return 0;
		}

		for (uint i = 0; i != maxNROchs; i++) {
			hit[i] = 0;
		}

		/**
		 * Get Arrays with hit Info
		 */
		const uint64_t* const edgeTime = richPacket->getTimes();
		const uint_fast8_t* const edgeChID = richPacket->getChIDs();
		const bool* const edgeIsLeading = richPacket->getIsLeadings();
		const uint_fast8_t* const edgeTdcID = richPacket->getTdcIDs();
		const uint_fast16_t edgeTrbID = richPacket->getFragmentNumber();
		double time, dtL0TP, dtCHOD;

		uint numberOfEdgesOfCurrentBoard = richPacket->getNumberOfEdgesStored();

//		LOG_INFO("LAV: Tel62 ID " << lavPacket->getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard);

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//			LOG_INFO("Edge " << iEdge << " ID " << edgeIsLeading[iEdge]);
//			LOG_INFO("Edge " << iEdge << " chID " << (uint) edgeChID[iEdge]);
//			LOG_INFO("Edge " << iEdge << " tdcID " << (uint) edgeTdcID[iEdge]);
//			LOG_INFO("Edge " << iEdge << " time " << std::hex << edgeTime[iEdge] << std::dec);
		}
	}
	return 1;
}

void RICHMultiTrackAlgo::writeData(L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {

	if (AlgoID_ != algoPacket->algoID)
		LOG_ERROR("Algo ID does not match with Algo ID already written within the packet!");

	algoPacket->algoID = AlgoID_;
	algoPacket->onlineTimeWindow = (uint) AlgoOnlineTimeWindow_[l0MaskID];
//	algoPacket->qualityFlags = (l1Info->isL1LAVProcessed() << 6) | (l1Info->isL1LAVEmptyPacket() << 4) | (l1Info->isL1LAVBadData() << 2) | AlgoRefTimeSourceID_[l0MaskID];
	algoPacket->qualityFlags = (l1Info->isL1LAVProcessed() << 6) | (l1Info->isL1LAVEmptyPacket() << 4) | (l1Info->isL1LAVBadData() << 2)
			| ((uint) l1Info->getL1LAVTrgWrd(l0MaskID));

	//algoPacket->l1Data[0] = l1Info->getSomethingRICHRelated();
	algoPacket->l1Data[0] = 0;
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

} /* namespace na62 */
