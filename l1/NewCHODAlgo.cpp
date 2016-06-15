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

uint NewCHODAlgo::algoID; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
uint NewCHODAlgo::algoLogic[16];
uint NewCHODAlgo::algoRefTimeSourceID[16];
double NewCHODAlgo::algoOnlineTimeWindow[16];

bool NewCHODAlgo::algoProcessed = 0;
bool NewCHODAlgo::emptyPacket = 0;
bool NewCHODAlgo::badData = 0;
bool NewCHODAlgo::isNewCHODRefTime = 0;

double NewCHODAlgo::averageHitTime = 0.;
NewCHODParsConfFile* NewCHODAlgo::infoNewCHOD_ =
		NewCHODParsConfFile::GetInstance();
int * NewCHODAlgo::PMTGeo = infoNewCHOD_->getGeoPMTMap();
uint NewCHODAlgo::nHits;
uint NewCHODAlgo::nMaxPMTs;
int NewCHODAlgo::PMTID1;
int NewCHODAlgo::PMTID2;

NewCHODAlgo::NewCHODAlgo() {
}

NewCHODAlgo::~NewCHODAlgo() {
// TODO Auto-generated destructor stub
}

/*void NewCHODAlgo::initialize(uint i, l1NewCHOD &l1NewCHODStruct) {

 algoID = l1NewCHODStruct.configParams.l1TrigMaskID;
 algoLogic[i] = l1NewCHODStruct.configParams.l1TrigLogic;
 algoRefTimeSourceID[i] = l1NewCHODStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
 algoOnlineTimeWindow[i] = l1NewCHODStruct.configParams.l1TrigOnlineTimeWindow;
 //	LOG_INFO("NewCHOD mask: " << i << " logic " << algoLogic[i] << " refTimeSourceID " << algoRefTimeSourceID[i] << " online time window " << algoOnlineTimeWindow[i]);
 }*/

uint_fast8_t NewCHODAlgo::processNewCHODTrigger(uint l0MaskID,
		DecoderHandler& decoder, L1InfoToStorage* l1Info) {

//	LOG_INFO("Initial Time " << time[0].tv_sec << " " << time[0].tv_usec);

	using namespace l0;

	nMaxPMTs = 6;
	nHits = 0;
	averageHitTime = 0;

//	LOG_INFO("Event number = " << decoder.getDecodedEvent()->getEventNumber());
//	LOG_INFO("NewCHODAlgo: event timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);
//	LOG_INFO("Average Hit Time (initial value) " << averageHitTime);

	TrbFragmentDecoder& newchodPacket =
			(TrbFragmentDecoder&) decoder.getDecodedIRCFragment(0);
	if (!newchodPacket.isReady() || newchodPacket.isBadFragment()) {
//
		LOG_ERROR("NewCHODAlgo: This looks like a Bad Packet!!!! ");
		badData = 1;
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
	double finetime, edgetime1, edgetime2;

	uint numberOfEdgesOfCurrentBoard = newchodPacket.getNumberOfEdgesStored();
	if (!numberOfEdgesOfCurrentBoard)
		emptyPacket = 1;

	for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {
		const int roChID = (edge_tdcIDs[iEdge] * 32) + edge_chIDs[iEdge];
		PMTID1 = PMTGeo[roChID];
//		LOG_INFO("Readout Channel ID1 " << roChID);
//		LOG_INFO("Geom PMT ID1 " << PMTID1);
		if ((PMTID1 / 10) % 10 >= 1 && (PMTID1 / 10) % 10 <= 3) {
			for (uint jEdge = 0; jEdge != numberOfEdgesOfCurrentBoard;
					jEdge++) {
				const int roChID = (edge_tdcIDs[jEdge] * 32)
						+ edge_chIDs[jEdge];
				PMTID2 = PMTGeo[roChID];
				//			LOG_INFO("Readout Channel ID2 " << roChID);
				//			LOG_INFO("Geom PMT ID2 " << PMTID2);
				if (fabs(PMTID1 - PMTID2) == 50
						&& (PMTID1 / 100) % 10 == (PMTID2 / 100) % 10) {
					finetime = decoder.getDecodedEvent()->getFinetime()
							* 0.097464731802;
					edgetime1 = (edge_times[iEdge]
							- decoder.getDecodedEvent()->getTimestamp() * 256.)
							* 0.097464731802;
					edgetime2 = (edge_times[jEdge]
							- decoder.getDecodedEvent()->getTimestamp() * 256.)
							* 0.097464731802;
					if (fabs(edgetime1 - edgetime2) < 5) {
//						LOG_INFO("edge1-edge2 "<<fabs(edgetime1-edgetime2));
						return 1;
					}
				}
			}
		}
	}

	algoProcessed = 1;

	//l1Info->setNewCHODAverageTime(averageHitTime);
	//l1Info->setL1NewCHODProcessed();

//	LOG_INFO("NewCHODAlgo=============== number of Hits " << nHits_V + nHits_H);
//	LOG_INFO("NewCHODAlgo=============== average HitTime " << averageHitTime);
//	LOG_INFO("NewCHODAlgo=============== L1NewCHODProcessed Flag " << (uint)l1Info->isL1NewCHODProcessed());
//	LOG_INFO("NewCHODAlgo=============== reset average HitTime " << averageHitTime);

	/*	if (algoLogic)
	 return ((nHits > 0) && ((nHits) < nMaxPMTs));
	 else
	 return (nHits >= nMaxPMTs);*/
}

bool NewCHODAlgo::isAlgoProcessed() {
	return algoProcessed;
}

void NewCHODAlgo::resetAlgoProcessed() {
	algoProcessed = 0;
}

bool NewCHODAlgo::isEmptyPacket() {
	return emptyPacket;
}

bool NewCHODAlgo::isBadData() {
	return badData;
}

void NewCHODAlgo::writeData(L1Block &l1Block) {

	int numToMaskID;
	for (int iNum = 0; iNum < L1TriggerProcessor::GetNumberOfEnabledL0Masks(); iNum++) {
		numToMaskID = L1TriggerProcessor::GetL0MaskNumToMaskID(iNum);
		if (numToMaskID == -1)
			LOG_ERROR("ERROR! Wrong association of mask ID!");
		(l1Block.l1Mask[iNum]).l1Algo[algoID].l1AlgoID = algoID;
		(l1Block.l1Mask[iNum]).l1Algo[algoID].l1AlgoProcessed = algoProcessed;
		(l1Block.l1Mask[iNum]).l1Algo[algoID].l1AlgoOnlineTimeWindow =
				(uint) algoOnlineTimeWindow[numToMaskID];
	}
}
}
/* namespace na62 */
