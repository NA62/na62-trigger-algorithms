/*
 * NewCHODAlgo.cpp
 *
 *  Created on: 16 May 2016
 *      Author: LorenzaIacobuzio
 */

#include "NewCHODAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#include <options/Logging.h>

#include "../common/decoding/TrbFragmentDecoder.h"
#include "newchod_algorithm/ParsConfFile.h"
#include "L1TriggerProcessor.h"

namespace na62 {

uint NewCHODAlgo::AlgoID_; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
uint NewCHODAlgo::AlgoLogic_[16];
uint NewCHODAlgo::AlgoRefTimeSourceID_[16];
double NewCHODAlgo::AlgoOnlineTimeWindow_[16];

NewCHODParsConfFile* NewCHODAlgo::InfoNewCHOD_ = NewCHODParsConfFile::GetInstance();
int * NewCHODAlgo::PmtGeo_ = InfoNewCHOD_->getGeoPMTMap();

void NewCHODAlgo::initialize(uint i, l1NewCHOD &l1NewChodStruct) {

	AlgoID_ = l1NewChodStruct.configParams.l1TrigMaskID;
	AlgoLogic_[i] = l1NewChodStruct.configParams.l1TrigLogic;
	AlgoRefTimeSourceID_[i] = l1NewChodStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	AlgoOnlineTimeWindow_[i] = l1NewChodStruct.configParams.l1TrigOnlineTimeWindow;
// 	LOG_INFO("NewCHOD mask: " << i << " logic " << AlgoLogic_[i] << " refTimeSourceID " << AlgoRefTimeSourceID_[i] << " online time window " << AlgoOnlineTimeWindow_[i]);
}

uint_fast8_t NewCHODAlgo::processNewCHODTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info) {

	using namespace l0;

//	LOG_INFO("NewCHOD: event timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);
//	LOG_INFO("NewCHOD: event reference fine time from L0TP " << std::hex << (uint)decoder.getDecodedEvent()->getFinetime() << std::dec);

	uint nMaxPMTs = 4;
	uint nHits = 0;
	double averageHitTime = 0.;
//	LOG_INFO("Average Hit Time (initial value) " << averageHitTime);

	double refTimeL0TP = decoder.getDecodedEvent()->getFinetime() * 0.097464731802;
//	LOG_INFO("L1 reference finetime from L0TP (ns) " << refTimeL0TP);

	TrbFragmentDecoder& newchodPacket = (TrbFragmentDecoder&) decoder.getDecodedIRCFragment(0);

	if (!newchodPacket.isReady() || newchodPacket.isBadFragment()) {
		LOG_ERROR("NewCHODAlgo: This looks like a Bad Packet!!!! ");
		l1Info->setL1NewCHODBadData();
		return 0;
	}

	/**
	 * Get Arrays with hit Info
	 */
	const uint64_t* const edgeTime = newchodPacket.getTimes();
	const uint_fast8_t* const edgeChID = newchodPacket.getChIDs();
	const bool* const edgeIsLeading = newchodPacket.getIsLeadings();
	const uint_fast8_t* const edgeTdcID = newchodPacket.getTdcIDs();
	double time1, time2, dt1L0TP, dt2L0TP;
	int PMTID1, PMTID2;

	uint numberOfEdgesOfCurrentBoard = newchodPacket.getNumberOfEdgesStored();

	if (!numberOfEdgesOfCurrentBoard)
		l1Info->setL1NewCHODEmptyPacket();

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
			const int roChID1 = (edgeTdcID[iEdge] * 32) + edgeChID[iEdge];
			if (roChID1 >= 192) {
				PMTID1 = PmtGeo_[roChID1];
//			LOG_INFO("Readout Channel ID1 " << roChID1);
//			LOG_INFO("Geom PMT ID1 " << PMTID1);

				if ((PMTID1 / 10) % 10 >= 0 && (PMTID1 / 10) % 10 <= 3) {
					for (uint jEdge = 0; jEdge != numberOfEdgesOfCurrentBoard; jEdge++) {
						if (edgeIsLeading[jEdge] && jEdge != iEdge) {
							const int roChID2 = (edgeTdcID[jEdge] * 32) + edgeChID[jEdge];
							if (roChID2 >= 192) {
								PMTID2 = PmtGeo_[roChID2];
//						LOG_INFO("Readout Channel ID2 " << roChID2);
//						LOG_INFO("Geom PMT ID2 " << PMTID2);

								if (fabs(PMTID1 - PMTID2) == 50 && (PMTID1 / 100) % 10 == (PMTID2 / 100) % 10) {
									time1 = (edgeTime[iEdge] - decoder.getDecodedEvent()->getTimestamp() * 256.) * 0.097464731802;
									time2 = (edgeTime[jEdge] - decoder.getDecodedEvent()->getTimestamp() * 256.) * 0.097464731802;
//							LOG_INFO("edgetime1 (in ns) " << time1 << " edgetime2 " << time2);

									dt1L0TP = fabs(time1 - refTimeL0TP);
									dt2L0TP = fabs(time2 - refTimeL0TP);
//							LOG_INFO("dt1L0TP "<< dt1L0TP << " dt2L0TP " << dt2L0TP);

									if ((dt1L0TP < AlgoOnlineTimeWindow_[l0MaskID]) && (dt2L0TP < AlgoOnlineTimeWindow_[l0MaskID])
											&& (fabs(time1 - time2) < 5.)) {

										if (AlgoRefTimeSourceID_[l0MaskID] == 1)
											averageHitTime += time1;

										nHits++;
									}
								}
							}
						}
					}
				}
			}
		}
	}

//	LOG_INFO("NewCHOD: Analysing Event " << decoder.getDecodedEvent()->getEventNumber());
//	LOG_INFO("Timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

	if ((AlgoRefTimeSourceID_[l0MaskID] == 1) && nHits) {
		averageHitTime = averageHitTime / (nHits);
	} else
		averageHitTime = -1.0e+28;

	l1Info->setNewCHODAverageTime(averageHitTime);
	l1Info->setL1NewCHODNHits(nHits);
	l1Info->setL1NewCHODProcessed();

//	LOG_INFO("NewCHODAlgo=============== number of Hits " << nHits);
//	LOG_INFO("NewCHODAlgo=============== average HitTime " << averageHitTime);

	return ((nHits > 0) && (nHits < nMaxPMTs));

}

void NewCHODAlgo::writeData(L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {

	if (AlgoID_ != algoPacket->algoID)
		LOG_ERROR("Algo ID does not match with Algo ID already written within the packet!");

	algoPacket->algoID = AlgoID_;
	algoPacket->onlineTimeWindow = (uint) AlgoOnlineTimeWindow_[l0MaskID];
//	algoPacket->qualityFlags = (l1Info->isL1NewCHODProcessed() << 6) | (l1Info->isL1NewCHODEmptyPacket() << 4) | (l1Info->isL1NewCHODBadData() << 2) | AlgoRefTimeSourceID_[l0MaskID];
	algoPacket->qualityFlags = (l1Info->isL1NewCHODProcessed() << 6) | (l1Info->isL1NewCHODEmptyPacket() << 4)
			| (l1Info->isL1NewCHODBadData() << 2) | ((uint) l1Info->getL1NewCHODTrgWrd());

	algoPacket->l1Data[0] = (uint) l1Info->getL1NewCHODNHits();
	if (!AlgoRefTimeSourceID_[l0MaskID]) {
		algoPacket->l1Data[1] = l1Info->getL1RefTimeL0TP();
	} else if (AlgoRefTimeSourceID_[l0MaskID] == 1) {
		algoPacket->l1Data[1] = l1Info->getNewCHODAverageTime(); //this is a double!!!
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
