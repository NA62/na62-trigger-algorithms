/*
 * KtagAlgo.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk 
 */

#include "KtagAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <options/Logging.h>
#include <string.h>
#include <math.h>

#include <sys/time.h>

#include "../common/decoding/DecoderRange.h"
#include "../common/decoding/DecoderHandler.h"
#include "../common/decoding/TrbFragmentDecoder.h"
#include "L1TriggerProcessor.h"

#define maxNhits 500

namespace na62 {

uint KtagAlgo::AlgoID_; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
uint KtagAlgo::AlgoLogic_[16];
uint KtagAlgo::AlgoRefTimeSourceID_[16];
double KtagAlgo::AlgoOnlineTimeWindow_[16];

void KtagAlgo::initialize(uint i, l1KTAG &l1KtagStruct) {

	AlgoID_ = l1KtagStruct.configParams.l1TrigMaskID;
	AlgoLogic_[i] = l1KtagStruct.configParams.l1TrigLogic;
	AlgoRefTimeSourceID_[i] = l1KtagStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	AlgoOnlineTimeWindow_[i] = l1KtagStruct.configParams.l1TrigOnlineTimeWindow;
//	LOG_INFO("KTAG: mask: " << i << " logic " << AlgoLogic_[i] << " refTimeSourceID " << AlgoRefTimeSourceID_[i] << " online time window " << AlgoOnlineTimeWindow_[i]);
}

uint_fast8_t KtagAlgo::processKtagTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info) {

	using namespace l0;
//  LOG_INFO("Initial Time " << time[0].tv_sec << " " << time[0].tv_usec);
//	LOG_INFO("KTAGAlgo: event timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

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
			LOG_ERROR("KtagAlgo.cpp: Not able to use averageCHODHitTime as Reference Time even if it is requested!");
	}

	uint sector_occupancy_chod[8] = { 0 };
	uint sector_occupancy_l0tp[8] = { 0 };

	uint nEdges_tot = 0;

//TODO: chkmax need to be USED
	DecoderRange<TrbFragmentDecoder> x = decoder.getCEDARDecoderRange();
	if (x.begin() == x.end()) {
		LOG_ERROR("KTAG: Empty decoder range!");
		l1Info->setL1KTAGBadData();
//		badData = 1;
		return 0;
	}

	for (TrbFragmentDecoder* cedarPacket : decoder.getCEDARDecoderRange()) {
//      LOG_INFO("First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec);

		if (!cedarPacket->isReady() || cedarPacket->isBadFragment()) {
			LOG_ERROR("KTAGAlgo: This looks like a Bad Packet!!!! ");
			l1Info->setL1KTAGBadData();
//			badData = 1;
			return 0;
		}

		/**
		 * Get Arrays with hit Info
		 */
		const uint64_t* const edge_times = cedarPacket->getTimes();
//		const uint_fast8_t* const edge_chIDs = cedarPacket->getChIDs();
		const bool* const edge_IDs = cedarPacket->getIsLeadings();
		const uint_fast8_t* const edge_tdcIDs = cedarPacket->getTdcIDs();
		double finetime, edgetime, dt_l0tp, dt_chod;

		uint numberOfEdgesOfCurrentBoard = cedarPacket->getNumberOfEdgesStored();

//		LOG_INFO("Tel62 ID " << cedarPacket->getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard);
//      LOG_INFO("time check (inside iterator - end of retrieve) " << time[2].tv_sec << " " << time[2].tv_usec);

//		LOG_INFO("Before loop on the edges KTag");
		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//			LOG_INFO("Edge " << iEdge << " ID " << edge_IDs[iEdge]);
//			LOG_INFO("Edge " << iEdge << " chID " << (uint) edge_chIDs[iEdge]);
//			LOG_INFO("Edge " << iEdge << " tdcID " << (uint) edge_tdcIDs[iEdge]);
//			LOG_INFO("Edge " << iEdge << " time " << std::hex << edge_times[iEdge] << std::dec);
//			LOG_INFO(decoder.getDecodedEvent()->getFinetime());
//			LOG_INFO(decoder.getDecodedEvent()->getBurstID());

			/**
			 * Process leading edges only
			 *
			 */
			if (edge_IDs[iEdge]) {
				edgetime = (edge_times[iEdge] - decoder.getDecodedEvent()->getTimestamp() * 256.) * 0.097464731802;

				if (!isCHODRefTime) {
					finetime = decoder.getDecodedEvent()->getFinetime() * 0.097464731802;
					dt_l0tp = fabs(edgetime - finetime);
					dt_chod = -1.0e+28;
				} else
					dt_chod = fabs(edgetime - averageCHODHitTime);
//				LOG_INFO("dt_l0tp " << dt_l0tp << " dt_chod " << dt_chod);
				if ((!isCHODRefTime && dt_l0tp < AlgoOnlineTimeWindow_[l0MaskID])
						|| (isCHODRefTime && dt_chod < AlgoOnlineTimeWindow_[l0MaskID])) {
					const uint trbID = edge_tdcIDs[iEdge] / 4;
					const uint box = calculateSector(cedarPacket->getFragmentNumber(), trbID);

					if (!isCHODRefTime)
						sector_occupancy_l0tp[box]++;
					else
						sector_occupancy_chod[box]++;
				}
			}
		}
//      LOG_INFO("time check (inside iterator - end of computations) " << time[3].tv_sec << " " << time[3].tv_usec);
		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}

	if (!nEdges_tot)
		l1Info->setL1KTAGEmptyPacket();
//		emptyPacket = 1;
//	LOG_INFO("KtagAlgo.cpp: Analysing Event " << decoder.getDecodedEvent()->getEventNumber() << " - Timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << " - Total Number of edges found " << nEdges_tot);
//  LOG_INFO("time check (outside for - all Tel62s)" << time[4].tv_sec << " " << time[4].tv_usec);

	uint nSectors_l0tp = 0;
	uint nSectors_chod = 0;
	for (uint iSec = 0; iSec != 8; iSec++) {
		if (!isCHODRefTime && sector_occupancy_l0tp[iSec])
			nSectors_l0tp++;
		if (isCHODRefTime && sector_occupancy_chod[iSec])
			nSectors_chod++;
	}
//	LOG_INFO("sectors_l0tp = " << nSectors_l0tp << " sectors_chod = " << nSectors_chod);
//	LOG_INFO("Angela: " << decoder.getDecodedEvent()->getEventNumber() << "\t" << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

//  LOG_INFO("time check (outside for - Nsector coincidences)" << time[5].tv_sec << " " << time[5].tv_usec);
//for (int i = 0; i < 5; i++) {
//             if(i && i!=3) LOG_INFO(((time[i+1].tv_sec - time[i].tv_sec)*1e6 + time[i+1].tv_usec) - time[i].tv_usec << " ");
//     }

//	LOG_INFO(std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << " " << nEdges_tot << " " << nSectors << " " << ((time[4].tv_sec - time[0].tv_sec)*1e6 + time[4].tv_usec) - time[0].tv_usec << " " << ((time[5].tv_sec - time[0].tv_sec)*1e6 + time[5].tv_usec) - time[0].tv_usec);
//	LOG_INFO(nEdges_tot << " " << ((time[4].tv_sec - time[0].tv_sec)*1e6 + time[4].tv_usec) - time[0].tv_usec << " " << ((time[5].tv_sec - time[0].tv_sec)*1e6 + time[5].tv_usec) - time[0].tv_usec);

	if (!isCHODRefTime)
		l1Info->setL1KTAGNSectors_l0tp(nSectors_l0tp);
	else
		l1Info->setL1KTAGNSectors_chod(nSectors_chod);
	l1Info->setL1KTAGProcessed();
//	algoProcessed = 1;

//	if (AlgoLogic_[l0MaskID])
	return ((!isCHODRefTime && nSectors_l0tp > 4) || (isCHODRefTime && nSectors_chod > 4));
//	else return ((!isCHODRefTime && nSectors_l0tp <= 4) || (isCHODRefTime && nSectors_chod <= 4));
}

void KtagAlgo::writeData(L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {

	if (AlgoID_ != algoPacket->algoID)
		LOG_ERROR("Algo ID does not match with Algo ID written within the packet!");
	algoPacket->algoID = AlgoID_;
	algoPacket->onlineTimeWindow = (uint) AlgoOnlineTimeWindow_[l0MaskID];
//	algoPacket->qualityFlags = (l1Info->isL1KTAGProcessed() << 6) | (l1Info->isL1KTAGEmptyPacket() << 4) | (l1Info->isL1KTAGBadData() << 2) | AlgoRefTimeSourceID_[l0MaskID];
	algoPacket->qualityFlags = (l1Info->isL1KTAGProcessed() << 6) | (l1Info->isL1KTAGEmptyPacket() << 4) | (l1Info->isL1KTAGBadData() << 2)
			| ((uint)l1Info->getL1KTAGTrgWrd());
	if (AlgoRefTimeSourceID_[l0MaskID] == 1)
		algoPacket->l1Data[0] = l1Info->getL1KTAGNSectors_chod();
	else
		algoPacket->l1Data[0] = l1Info->getL1KTAGNSectors_l0tp();
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

