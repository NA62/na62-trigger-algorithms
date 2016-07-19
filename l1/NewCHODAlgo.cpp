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

//	LOG_INFO("Initial Time " << time[0].tv_sec << " " << time[0].tv_usec);

	using namespace l0;

	uint nMaxPMTs = 4;
	uint nHits = 0;
	double averageHitTime = 0.;

//	LOG_INFO("Event number = " << decoder.getDecodedEvent()->getEventNumber());
//	LOG_INFO("NewCHODAlgo: event timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);
//	LOG_INFO("Average Hit Time (initial value) " << averageHitTime);

	TrbFragmentDecoder& newchodPacket = (TrbFragmentDecoder&) decoder.getDecodedIRCFragment(0);
	if (!newchodPacket.isReady() || newchodPacket.isBadFragment()) {
//
		LOG_ERROR("NewCHODAlgo: This looks like a Bad Packet!!!! ");
		l1Info->setL1NewCHODBadData();
//		badData = 1;
		return 0;
	}
//	LOG_INFO("First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec);

	/**
	 * Get Arrays with hit Info
	 */
	const uint64_t* const edge_times = newchodPacket.getTimes();
	const uint_fast8_t* const edge_chIDs = newchodPacket.getChIDs();
	const bool* const edge_IDs = newchodPacket.getIsLeadings();
	const uint_fast8_t* const edge_tdcIDs = newchodPacket.getTdcIDs();
	double finetime, edgetime1, edgetime2, dt1_l0tp, dt2_l0tp;
	int PMTID1, PMTID2;

	uint numberOfEdgesOfCurrentBoard = newchodPacket.getNumberOfEdgesStored();
	if (!numberOfEdgesOfCurrentBoard)
		l1Info->setL1NewCHODEmptyPacket();
//		emptyPacket = 1;

	for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
		/**
		 * Process leading edges only
		 *
		 */
		if (edge_IDs[iEdge]) {
			const int roChID1 = (edge_tdcIDs[iEdge] * 32) + edge_chIDs[iEdge];
			PMTID1 = PmtGeo_[roChID1];
//			LOG_INFO("iEdge " << iEdge);
//			LOG_INFO("Readout Channel ID1 " << roChID1);
//			LOG_INFO("Geom PMT ID1 " << PMTID1);

			if ((PMTID1 / 10) % 10 >= 1 && (PMTID1 / 10) % 10 <= 3) {
				for (uint jEdge = 0; jEdge != numberOfEdgesOfCurrentBoard; jEdge++) {
					if (edge_IDs[jEdge] && jEdge != iEdge) {
						const int roChID2 = (edge_tdcIDs[jEdge] * 32) + edge_chIDs[jEdge];
						PMTID2 = PmtGeo_[roChID2];
//						LOG_INFO("jEdge " << jEdge);
//						LOG_INFO("Readout Channel ID2 " << roChID2);
//						LOG_INFO("Geom PMT ID2 " << PMTID2);

						if (fabs(PMTID1 - PMTID2) == 50 && (PMTID1 / 100) % 10 == (PMTID2 / 100) % 10) {
							finetime = decoder.getDecodedEvent()->getFinetime() * 0.097464731802;
							edgetime1 = (edge_times[iEdge] - decoder.getDecodedEvent()->getTimestamp() * 256.) * 0.097464731802;
							edgetime2 = (edge_times[jEdge] - decoder.getDecodedEvent()->getTimestamp() * 256.) * 0.097464731802;
//							LOG_INFO("finetime (in ns) " << finetime << " edgetime1 (in ns) " << edgetime1 << " edgetime2 " << edgetime2);

							dt1_l0tp = fabs(edgetime1 - finetime);
							dt2_l0tp = fabs(edgetime2 - finetime);

//							LOG_INFO("dt1_l0tp "<< dt1_l0tp << " dt2_l0tp " << dt2_l0tp);
//							LOG_INFO("edge1-edge2 "<<fabs(edgetime1-edgetime2));
							if ((dt1_l0tp < AlgoOnlineTimeWindow_[l0MaskID]) && (dt2_l0tp < AlgoOnlineTimeWindow_[l0MaskID])
									&& (fabs(edgetime1 - edgetime2) < 5.)) {

								if (AlgoRefTimeSourceID_[l0MaskID] == 1)
									averageHitTime += edgetime1;
								nHits++;
							}
						}
					}
				}
			}
		}
	}

	if ((AlgoRefTimeSourceID_[l0MaskID] == 1) && nHits) {
		averageHitTime = averageHitTime / (nHits);
	} else
		averageHitTime = -1.0e+28;

	l1Info->setNewCHODAverageTime(averageHitTime);
	l1Info->setL1NewCHODNHits(nHits);
	l1Info->setL1NewCHODProcessed();

//	LOG_INFO("NewCHODAlgo=============== number of Hits " << nHits);
//	LOG_INFO("NewCHODAlgo=============== average HitTime " << averageHitTime);
//	LOG_INFO("NewCHODAlgo=============== L1NewCHODProcessed Flag " << (uint)l1Info->isL1NewCHODProcessed());
//	if (AlgoLogic_[l0MaskID])
	return ((nHits > 0) && (nHits < nMaxPMTs));
//	else return (nHits >= nMaxPMTs);

}

void NewCHODAlgo::writeData(L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {

	if (AlgoID_ != algoPacket->algoID)
		LOG_ERROR("Algo ID does not match with Algo ID written within the packet!");
	algoPacket->algoID = AlgoID_;
	algoPacket->onlineTimeWindow = (uint) AlgoOnlineTimeWindow_[l0MaskID];
//	algoPacket->qualityFlags = (l1Info->isL1NewCHODProcessed() << 6) | (l1Info->isL1NewCHODEmptyPacket() << 4) | (l1Info->isL1NewCHODBadData() << 2) | AlgoRefTimeSourceID_[l0MaskID];
	algoPacket->qualityFlags = (l1Info->isL1NewCHODProcessed() << 6) | (l1Info->isL1NewCHODEmptyPacket() << 4)
			| (l1Info->isL1NewCHODBadData() << 2) | ((uint) l1Info->getL1NewCHODTrgWrd());
	algoPacket->l1Data[0] = (uint) l1Info->getL1NewCHODNHits();
	if (AlgoRefTimeSourceID_[l0MaskID] == 1)
		algoPacket->l1Data[1] = l1Info->getNewCHODAverageTime();
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
