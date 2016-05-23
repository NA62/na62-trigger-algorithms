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

uint KtagAlgo::algoID; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for MUV3, 5 for Straw
uint KtagAlgo::algoLogic;
uint KtagAlgo::algoRefTimeSourceID;
double KtagAlgo::algoOnlineTimeWindow;

bool KtagAlgo::algoProcessed = 0;
bool KtagAlgo::emptyPacket = 0;
bool KtagAlgo::badData = 0;
bool KtagAlgo::isCHODRefTime = 0;
double KtagAlgo::averageCHODHitTime = 0.;
uint_fast8_t KtagAlgo::numberOfEnabledL0Masks = 0;

void KtagAlgo::initialize(l1KTAG &l1KtagStruct, uint_fast8_t nEnabledMasks) {

	algoID = l1KtagStruct.configParams.l1TrigMaskID;
	algoLogic = l1KtagStruct.configParams.l1TrigLogic;
	algoRefTimeSourceID = l1KtagStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	algoOnlineTimeWindow = l1KtagStruct.configParams.l1TrigOnlineTimeWindow;
	numberOfEnabledL0Masks = nEnabledMasks;

}

uint_fast8_t KtagAlgo::processKtagTrigger(DecoderHandler& decoder,
		L1InfoToStorage* l1Info) {

	using namespace l0;
//  LOG_INFO("Initial Time " << time[0].tv_sec << " " << time[0].tv_usec);
//	LOG_INFO("KTAGAlgo: event timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);

	/*
	 * TODO: The same logic needs to be developed for RICHRefTime
	 */
	if (algoRefTimeSourceID == 1) {
		if (l1Info->isL1CHODProcessed() && averageCHODHitTime != -1.0e+28) {
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
		LOG_ERROR ("KTAG: Empty decoder range!");
		badData = 1;
		return 0;
	}

	for (TrbFragmentDecoder* cedarPacket : decoder.getCEDARDecoderRange()) {
//      LOG_INFO("First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec);

		if (!cedarPacket->isReady() || cedarPacket->isBadFragment()) {
			LOG_ERROR ("KTAGAlgo: This looks like a Bad Packet!!!! ");
			badData = 1;
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

		uint numberOfEdgesOfCurrentBoard =
		cedarPacket->getNumberOfEdgesStored();

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

				if ((!isCHODRefTime && dt_l0tp < algoOnlineTimeWindow)
				|| (isCHODRefTime && dt_chod < algoOnlineTimeWindow)) {
					const uint trbID = edge_tdcIDs[iEdge] / 4;
					const uint box = calculateSector(
					cedarPacket->getFragmentNumber(), trbID);

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
		emptyPacket = 1;
//	LOG_INFO("KtagAlgo.cpp: Analysing Event " << decoder.getDecodedEvent()->getEventNumber() << " - Timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec << " - Total Number of edges found " << nEdges_tot);
//  LOG_INFO("time check (outside for - all Tel62s)" << time[4].tv_sec << " " << time[4].tv_usec);

	uint nSectors_chod = 0;
	uint nSectors_l0tp = 0;
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

	algoProcessed = 1;
	if (algoLogic)
		return ((!isCHODRefTime && nSectors_l0tp > 4)
				|| (isCHODRefTime && nSectors_chod > 4));
	else
		return ((!isCHODRefTime && nSectors_l0tp <= 4)
				|| (isCHODRefTime && nSectors_chod <= 4));
}

bool KtagAlgo::isAlgoProcessed() {
	return algoProcessed;
}

void KtagAlgo::resetAlgoProcessed() {
	algoProcessed = 0;
}

bool KtagAlgo::isEmptyPacket() {
	return emptyPacket;
}

bool KtagAlgo::isBadData() {
	return badData;
}

void KtagAlgo::writeData(L1Block &l1Block){

	for(int iNum=0; iNum<numberOfEnabledL0Masks; iNum++){
	  (l1Block.l1Mask[iNum]).l1Algo[algoID].l1AlgoID = algoID;
	  (l1Block.l1Mask[iNum]).l1Algo[algoID].l1AlgoProcessed = algoProcessed;
	  (l1Block.l1Mask[iNum]).l1Algo[algoID].l1AlgoOnlineTimeWindow = algoOnlineTimeWindow;
	}
}

}
/* namespace na62 */

