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
#include <sys/time.h>

#include <options/Logging.h>

#include "../common/decoding/TrbFragmentDecoder.h"
#include "chod_algorithm/ParsConfFile.h"
#include "L1TriggerProcessor.h"

namespace na62 {

uint CHODAlgo::AlgoID_; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
uint CHODAlgo::AlgoLogic_[16];
uint CHODAlgo::AlgoRefTimeSourceID_[16];
double CHODAlgo::AlgoOnlineTimeWindow_[16];

CHODParsConfFile* CHODAlgo::InfoCHOD_ = CHODParsConfFile::GetInstance();
int * CHODAlgo::SlabGeo_ = InfoCHOD_->getGeoSlabMap();

void CHODAlgo::initialize(uint i, l1CHOD &l1CHODStruct) {

	AlgoID_ = l1CHODStruct.configParams.l1TrigMaskID;
	AlgoLogic_[i] = l1CHODStruct.configParams.l1TrigLogic;
	AlgoRefTimeSourceID_[i] = l1CHODStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	AlgoOnlineTimeWindow_[i] = l1CHODStruct.configParams.l1TrigOnlineTimeWindow;
//	LOG_INFO("CHOD: mask " << i << " logic " << AlgoLogic_[i] << " refTimeSourceID " << AlgoRefTimeSourceID_[i] << " online time window " << AlgoOnlineTimeWindow_[i]);
}

uint_fast8_t CHODAlgo::processCHODTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info) {

//	LOG_INFO("Initial Time " << time[0].tv_sec << " " << time[0].tv_usec);

	using namespace l0;

//	LOG_INFO("CHOD: event timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);
//	LOG_INFO("CHOD: event reference fine time from L0TP " << std::hex << (uint)decoder.getDecodedEvent()->getFinetime() << std::dec);

	uint nMaxSlabs = 6;
	uint nHitsH = 0;
	uint nHitsV = 0;
	double averageHitTime = 0.;
//	LOG_INFO("Average Hit Time (initial value) " << averageHitTime);

	double refTimeL0TP = decoder.getDecodedEvent()->getFinetime() * 0.097464731802;
//	LOG_INFO("L1 reference finetime from L0TP (ns) " << refTimeL0TP);

	TrbFragmentDecoder& chodPacket = (TrbFragmentDecoder&) decoder.getDecodedCHODFragment(0);

	if (!chodPacket.isReady() || chodPacket.isBadFragment()) {
		LOG_ERROR("CHOD: This looks like a Bad Packet!!!! ");
		l1Info->setL1CHODBadData();
		return 0;
	}
//	LOG_INFO("First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec);

	/**
	 * Get Arrays with hit Info
	 */
	const uint64_t* const edgeTime = chodPacket.getTimes();
	const uint_fast8_t* const edgeChID = chodPacket.getChIDs();
	const bool* const edgeIsLeading = chodPacket.getIsLeadings();
	const uint_fast8_t* const edgeTdcID = chodPacket.getTdcIDs();
	double time, dtL0TP;
//	int quadrantID;
	int slabID, planeID;

	uint numberOfEdgesOfCurrentBoard = chodPacket.getNumberOfEdgesStored();

	if (!numberOfEdgesOfCurrentBoard)
		l1Info->setL1CHODEmptyPacket();

	for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//		LOG_INFO("Edge " << iEdge << " ID " << edgeIsLeading[iEdge]);
//		LOG_INFO("Edge " << iEdge << " chID " << (uint) edgeChID[iEdge]);
//		LOG_INFO("Edge " << iEdge << " tdcID " << (uint) edgeTdcID[iEdge]);
//		LOG_INFO("Edge " << iEdge << " time " << std::hex << edgeTime[iEdge] << std::dec);

		/**
		 * Process leading edges only
		 *
		 */
		if (edgeIsLeading[iEdge]) {
			const int roChID = (edgeTdcID[iEdge] * 32) + edgeChID[iEdge];
//			LOG_INFO("Readout Channel ID " << roChID);
//			LOG_INFO("Geom Slab ID " << SlabGeo_[roChID]);

			/**
			 * Process only first 128 readout channels, corresponding to low-threshold LAV FEE
			 *
			 */
			if (SlabGeo_[roChID] < 128) {
				time = (edgeTime[iEdge] - decoder.getDecodedEvent()->getTimestamp() * 256.) * 0.097464731802;
//				LOG_INFO("Edge time (ns) " << time);

				dtL0TP = fabs(time - refTimeL0TP);
//				LOG_INFO("dtL0TP " << dtL0TP);

				if (dtL0TP < AlgoOnlineTimeWindow_[l0MaskID]) {

					slabID = SlabGeo_[roChID];
//					quadrantID = slabID / 16.;
					planeID = slabID / 64.;
//					LOG_INFO("CHOD slab ID " << slabID);
//					LOG_INFO("CHOD quadrant ID " << quadrantID);
//					LOG_INFO("CHOD plane ID " << planeID);

					if (AlgoRefTimeSourceID_[l0MaskID] == 1)
						averageHitTime += time;

					if (planeID)
						nHitsV++;
					else
						nHitsH++;
				}
			}
		}
	}
//	LOG_INFO("time check " << time[2].tv_sec << " " << time[2].tv_usec);
//	LOG_INFO("CHOD: Analysing Event " << decoder.getDecodedEvent()->getEventNumber());
//	LOG_INFO("Timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);
//	LOG_INFO("time check (final)" << time[3].tv_sec << " " << time[3].tv_usec);

//	for (int i = 0; i < 3; i++) {
//		if (i)
//			LOG_INFO(((time[i+1].tv_sec - time[i].tv_sec)*1e6 + time[i+1].tv_usec) - time[i].tv_usec << " ");
//	}
//	LOG_INFO(((time[3].tv_sec - time[0].tv_sec)*1e6 + time[3].tv_usec) - time[0].tv_usec);

//	LOG_INFO("nHits(H) " << nHitsH << " nHits(V) " << nHitsV);

	if ((AlgoRefTimeSourceID_[l0MaskID] == 1) && (nHitsV + nHitsH)) {
		averageHitTime = averageHitTime / (nHitsV + nHitsH);
	} else
		averageHitTime = -1.0e+28;

	l1Info->setCHODAverageTime(averageHitTime);
	l1Info->setL1CHODNHits(nHitsV + nHitsH);
	l1Info->setL1CHODProcessed();

//	LOG_INFO("CHODAlgo=============== number of Hits " << nHitsV + nHitsH);
//	LOG_INFO("CHODAlgo=============== average HitTime " << averageHitTime);

	return (((nHitsV + nHitsH) > 0) && ((nHitsV + nHitsH) < nMaxSlabs));

//	return (((nHits_V == 1) && (nHits_H == 1))
//			|| ((nHits_V == 2) && (nHits_H == 1))
//			|| ((nHits_V == 1) && (nHits_H == 2)));

}

void CHODAlgo::writeData(L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {

	if (AlgoID_ != algoPacket->algoID)
		LOG_ERROR("Algo ID does not match with Algo ID already written within the packet!");

	algoPacket->algoID = AlgoID_;
	algoPacket->onlineTimeWindow = (uint) AlgoOnlineTimeWindow_[l0MaskID];
//	algoPacket->qualityFlags = (l1Info->isL1CHODProcessed() << 6) | (l1Info->isL1CHODEmptyPacket() << 4) | (l1Info->isL1CHODBadData() << 2) | AlgoRefTimeSourceID_[l0MaskID];
	algoPacket->qualityFlags = (l1Info->isL1CHODProcessed() << 6) | (l1Info->isL1CHODEmptyPacket() << 4) | (l1Info->isL1CHODBadData() << 2)
			| ((uint) l1Info->getL1CHODTrgWrd(l0MaskID));

	algoPacket->l1Data[0] = (uint) l1Info->getL1CHODNHits();
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

