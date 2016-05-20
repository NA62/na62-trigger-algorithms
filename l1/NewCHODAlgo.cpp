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

namespace na62 {

uint NewCHODAlgo::algoID; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for MUV3, 5 for Straw
uint NewCHODAlgo::algoLogic;
uint NewCHODAlgo::algoRefTimeSourceID;
double NewCHODAlgo::algoOnlineTimeWindow;

bool NewCHODAlgo::algoProcessed = 0;
bool NewCHODAlgo::emptyPacket = 0;
bool NewCHODAlgo::badData = 0;
bool NewCHODAlgo::isNewCHODRefTime = 0;

double NewCHODAlgo::averageHitTime = 0.;
NewCHODParsConfFile* NewCHODAlgo::infoNewCHOD_ = NewCHODParsConfFile::GetInstance();
int * NewCHODAlgo::PMTGeo = infoNewCHOD_->getGeoPMTMap();
uint NewCHODAlgo::nHits;
uint NewCHODAlgo::nMaxPMTs;
int NewCHODAlgo::PMTID1;
int NewCHODAlgo::PMTID2;
//int NewCHODAlgo::quadrantID;
//int NewCHODAlgo::planeID;

NewCHODAlgo::NewCHODAlgo() {
}

NewCHODAlgo::~NewCHODAlgo() {
// TODO Auto-generated destructor stub
}

/*void NewCHODAlgo::initialize(l1NewCHOD &l1NewCHODStruct) {

	algoID = l1NewCHODStruct.configParams.l1TrigMaskID;
	algoLogic = l1NewCHODStruct.configParams.l1TrigLogic;
	algoRefTimeSourceID = l1NewCHODStruct.configParams.l1TrigRefTimeSourceID; //0 for L0TP, 1 for CHOD, 2 for RICH
	algoOnlineTimeWindow = l1NewCHODStruct.configParams.l1TrigOnlineTimeWindow;

}*/

uint_fast8_t NewCHODAlgo::processNewCHODTrigger(DecoderHandler& decoder,
		L1InfoToStorage* l1Info) {

//	LOG_INFO("Initial Time " << time[0].tv_sec << " " << time[0].tv_usec);

	using namespace l0;

	nMaxPMTs = 6;
	nHits = 0;

//	LOG_INFO("Event number = " << decoder.getDecodedEvent()->getEventNumber());
//	LOG_INFO("NewCHODAlgo: event timestamp = " << std::hex << decoder.getDecodedEvent()->getTimestamp() << std::dec);
//	LOG_INFO("Average Hit Time (initial value) " << averageHitTime);

	TrbFragmentDecoder& newchodPacket =
			(TrbFragmentDecoder&) decoder.getDecodedNewCHODFragment(0);
	if (!newchodPacket.isReady() || newchodPacket.isBadFragment()) {

		LOG_ERROR ("NewCHODAlgo: This looks like a Bad Packet!!!! ");
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
		LOG_INFO("Readout Channel ID " << roChID);
		LOG_INFO("Geom PMT ID " << PMTID1);
		for (uint jEdge = 0; jEdge != numberOfEdgesOfCurrentBoard; jEdge++) {
			const int roChID = (edge_tdcIDs[jEdge] * 32) + edge_chIDs[jEdge];
			PMTID2 = PMTGeo[roChID];
			LOG_INFO("Readout Channel ID " << roChID);
			LOG_INFO("Geom PMT ID " << PMTID2);
			if (fabs(PMTID1-PMTID2) == 50) {
				finetime = decoder.getDecodedEvent()->getFinetime()
						* 0.097464731802;
				edgetime1 = (edge_times[iEdge]
						- decoder.getDecodedEvent()->getTimestamp() * 256.)
						* 0.097464731802;
				edgetime2 = (edge_times[jEdge]
						- decoder.getDecodedEvent()->getTimestamp() * 256.)
						* 0.097464731802;
				if(fabs(edgetime1-edgetime2) < 20) {
					averageHitTime += (edgetime1 + edgetime2)/2;
					nHits++;
				}
			}
		}
	}

	if (nHits)
		averageHitTime = averageHitTime/nHits;
	else
		averageHitTime = -1.0e+28;

	algoProcessed = 1;

	//l1Info->setNewCHODAverageTime(averageHitTime);
	//l1Info->setL1NewCHODProcessed();

//	LOG_INFO("NewCHODAlgo=============== number of Hits " << nHits_V + nHits_H);
//	LOG_INFO("NewCHODAlgo=============== average HitTime " << averageHitTime);
//	LOG_INFO("NewCHODAlgo=============== L1NewCHODProcessed Flag " << (uint)l1Info->isL1NewCHODProcessed());

	averageHitTime = 0;
//	LOG_INFO("NewCHODAlgo=============== reset average HitTime " << averageHitTime);

/*	if (algoLogic)
		return ((nHits > 0) && ((nHits) < nMaxPMTs));
	else
		return (nHits >= nMaxPMTs);*/
	if (nHits == 1)
		return averageHitTime;
	else
		return 0;
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

}
/* namespace na62 */
