/*
 * KtagAlgo.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk 
 */

#include "KtagAlgo.h"

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
//	LOG_INFO("KTAG: event timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);
//	LOG_INFO("KTAG: event reference fine time from L0TP " << std::hex << (uint)decoder.getDecodedEvent()->getFinetime() << std::dec);

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
			LOG_ERROR("KtagAlgo.cpp: Not able to use averageCHODHitTime as Reference Time even if requested!");
	} else
		LOG_ERROR("L1 Reference Time Source ID not recognised !!");

	uint sectorOccupancyCHOD[8] = { 0 };
	uint sectorOccupancyL0TP[8] = { 0 };
	uint nEdgesTot = 0;

	//TODO: chkmax need to be USED
	DecoderRange<TrbFragmentDecoder> x = decoder.getCEDARDecoderRange();
	if (x.begin() == x.end()) {
		LOG_ERROR("KTAG: Empty decoder range!");
		l1Info->setL1KTAGBadData();
		return 0;
	}

	for (TrbFragmentDecoder* cedarPacket : decoder.getCEDARDecoderRange()) {
//      LOG_INFO("First time check (inside iterator) " << time[1].tv_sec << " " << time[1].tv_usec);

		if (!cedarPacket->isReady() || cedarPacket->isBadFragment()) {
			LOG_ERROR("KTAG: This looks like a Bad Packet!!!! ");
			l1Info->setL1KTAGBadData();
			return 0;
		}

		/**
		 * Get Arrays with hit Info
		 */
		const uint64_t* const edgeTime = cedarPacket->getTimes();
		//const uint_fast8_t* const edgeChID = cedarPacket->getChIDs();
		const bool* const edgeIsLeading = cedarPacket->getIsLeadings();
		const uint_fast8_t* const edgeTdcID = cedarPacket->getTdcIDs();
		double time, dtL0TP, dtCHOD;

		uint numberOfEdgesOfCurrentBoard = cedarPacket->getNumberOfEdgesStored();

//		LOG_INFO("Tel62 ID " << cedarPacket->getFragmentNumber() << " - Number of Edges found " << numberOfEdgesOfCurrentBoard);
//      LOG_INFO("time check (inside iterator - end of retrieve) " << time[2].tv_sec << " " << time[2].tv_usec);

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
//			LOG_INFO("Edge " << iEdge << " ID " << edgeIsLeading[iEdge]);
//			LOG_INFO("Edge " << iEdge << " chID " << (uint) edgeChID[iEdge]);
//			LOG_INFO("Edge " << iEdge << " tdcID " << (uint) edgeTdcID[iEdge]);
//			LOG_INFO("Edge " << iEdge << " time " << std::hex << edgeTime[iEdge] << std::dec);

			/**
			 * Process leading edges only
			 *
			 */
			if (edgeIsLeading[iEdge]) {
				time = (edgeTime[iEdge] - decoder.getDecodedEvent()->getTimestamp() * 256.) * 0.097464731802;
//				LOG_INFO("Edge time (ns) " << time);

				if (!isCHODRefTime) {
					dtL0TP = fabs(time - refTimeL0TP);
					dtCHOD = -1.0e+28;
				} else
					dtCHOD = fabs(time - averageCHODHitTime);
//				LOG_INFO("dtL0TP " << dtL0TP << " dtCHOD " << dtCHOD);

				if ((!isCHODRefTime && dtL0TP < AlgoOnlineTimeWindow_[l0MaskID])
						|| (isCHODRefTime && dtCHOD < AlgoOnlineTimeWindow_[l0MaskID])) {
					const uint trbID = edgeTdcID[iEdge] / 4;
					const uint box = calculateSector(cedarPacket->getFragmentNumber(), trbID);

					if (!isCHODRefTime)
						sectorOccupancyL0TP[box]++;
					else
						sectorOccupancyCHOD[box]++;
				}
			}
		}
//      LOG_INFO("time check (inside iterator - end of computations) " << time[3].tv_sec << " " << time[3].tv_usec);
		nEdgesTot += numberOfEdgesOfCurrentBoard;
	}

	if (!nEdgesTot)
		l1Info->setL1KTAGEmptyPacket();

//	LOG_INFO("KTAG: Analysing Event " << decoder.getDecodedEvent()->getEventNumber());
//	LOG_INFO("Timestamp " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);
//	LOG_INFO("time check (outside for - all Tel62s)" << time[4].tv_sec << " " << time[4].tv_usec);

	uint nSectorsL0TP = 0;
	uint nSectorsCHOD = 0;
	for (uint iSec = 0; iSec != 8; iSec++) {
		if (!isCHODRefTime && sectorOccupancyL0TP[iSec])
			nSectorsL0TP++;
		if (isCHODRefTime && sectorOccupancyCHOD[iSec])
			nSectorsCHOD++;
	}
//	LOG_INFO("nEdgesTot " << nEdgesTot << " nSectorsL0TP " << nSectorsL0TP << " nSectorsCHOD " << nSectorsCHOD);

//	LOG_INFO("time check (outside for - Nsector coincidences)" << time[5].tv_sec << " " << time[5].tv_usec);
//	for (int i = 0; i < 5; i++) {
//		if (i && i != 3)
//			LOG_INFO(((time[i+1].tv_sec - time[i].tv_sec)*1e6 + time[i+1].tv_usec) - time[i].tv_usec << " ");
//	}
//	LOG_INFO(((time[4].tv_sec - time[0].tv_sec)*1e6 + time[4].tv_usec) - time[0].tv_usec << " " << ((time[5].tv_sec - time[0].tv_sec)*1e6 + time[5].tv_usec) - time[0].tv_usec);

	if (!isCHODRefTime)
		l1Info->setL1KTAGNSectorsL0TP(nSectorsL0TP);
	else
		l1Info->setL1KTAGNSectorsCHOD(nSectorsCHOD);

	l1Info->setL1KTAGProcessed();
	return ((!isCHODRefTime && nSectorsL0TP > 4) || (isCHODRefTime && nSectorsCHOD > 4));
}

void KtagAlgo::writeData(L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {

	if (AlgoID_ != algoPacket->algoID)
		LOG_ERROR("Algo ID does not match with Algo ID already written in the packet!");

	algoPacket->algoID = AlgoID_;
	algoPacket->onlineTimeWindow = (uint) AlgoOnlineTimeWindow_[l0MaskID];
//	algoPacket->qualityFlags = (l1Info->isL1KTAGProcessed() << 6) | (l1Info->isL1KTAGEmptyPacket() << 4) | (l1Info->isL1KTAGBadData() << 2) | AlgoRefTimeSourceID_[l0MaskID];
	algoPacket->qualityFlags = (l1Info->isL1KTAGProcessed() << 6) | (l1Info->isL1KTAGEmptyPacket() << 4) | (l1Info->isL1KTAGBadData() << 2)
			| ((uint) l1Info->getL1KTAGTrgWrd(l0MaskID));

	if (!AlgoRefTimeSourceID_[l0MaskID]) {
		algoPacket->l1Data[0] = l1Info->getL1KTAGNSectorsL0TP();
		algoPacket->l1Data[1] = l1Info->getL1RefTimeL0TP();
	} else if (AlgoRefTimeSourceID_[l0MaskID] == 1) {
		algoPacket->l1Data[0] = l1Info->getL1KTAGNSectorsCHOD();
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

