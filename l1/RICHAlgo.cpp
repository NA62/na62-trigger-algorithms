/*
 *
 RICHAlgo.cpp
 *
 *  Created on: 17 Apr 2015
 *      Author: Valeria Fascianelli
 */

#include "RICHAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <options/Logging.h>
#include <string.h>
#include <vector>

#include "../common/decoding/DecoderRange.h"
#include "../common/decoding/DecoderHandler.h"
#include "../common/decoding/TrbFragmentDecoder.h"
#include "rich_algorithm/ParsConfFile.h"
#include "rich_algorithm/RICHChannelID.h"

#define maxNhits 500

namespace na62 {

uint_fast8_t RICHAlgo::processRICHTrigger(DecoderHandler& decoder) {

	using namespace l0;

	LOG_INFO<<"===== RICHAlgo.cpp: Analysing Event ===" << decoder.getDecodedEvent()->getEventNumber() << ENDL;

	ParsConfFile* mapsChs = ParsConfFile::GetInstance();

	pmsGeo = mapsChs->getGeoPmsMap();
	pmsPos = mapsChs->getPosPmsMap();
	focalCenterJura = mapsChs->getFocalCenterJura();
	focalCenterSaleve = mapsChs->getFocalCenterSaleve();


	//LOG_INFO << "Geographical Channel " << chPos.at(1951) << ENDL;

	uint nEdges_tot = 0;
	uint chRO[maxNhits];

	vector<double> fitPositionX;
	vector<double> fitPositionY;

	for (TrbFragmentDecoder* richPacket : decoder.getRICHDecoderRange()) {

		LOG_INFO<< "Number of RICH Tel62s = " << decoder.getNumberOfRICHFragments() << ENDL;

		LOG_INFO<< "TEL62 ID = " << richPacket->getFragmentNumber() << ENDL;

		/**
		 * Get Arrays with hit Info
		 */

		const uint64_t* const edge_times = richPacket->getTimes();
		const uint_fast8_t* const edge_chIDs = richPacket->getChIDs();
		const bool* const edge_IDs = richPacket->getIsLeadings();
		const uint_fast8_t* const edge_tdcIDs = richPacket->getTdcIDs();
		const uint_fast16_t edge_trbIDs = richPacket->getFragmentNumber();

		uint numberOfEdgesOfCurrentBoard = richPacket->getNumberOfEdgesStored();

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {

			LOG_INFO<< "Edge " << iEdge + nEdges_tot << " ID " << (uint)edge_IDs[iEdge] << ENDL;
			LOG_INFO<< "Edge " << iEdge + nEdges_tot << " chID " << (uint)edge_chIDs[iEdge] << ENDL;
			LOG_INFO<< "Edge " << iEdge + nEdges_tot << " tdcID " << (uint)edge_tdcIDs[iEdge] << ENDL;
			LOG_INFO<< "Edge " << iEdge + nEdges_tot << " trbID " << (uint)edge_trbIDs << ENDL;
//			//LOG_INFO<< "Edge " << iEdge + nEdges_tot << " time " << std::hex << edge_times[iEdge] << std::dec << ENDL;

//==== Working with leading time only====

			if (edge_IDs[iEdge]) {

				chRO[iEdge] = edge_trbIDs * 512 + edge_tdcIDs[iEdge] * 32
				+ edge_chIDs[iEdge];

				RICHChannelID channel(pmsGeo[chRO[iEdge]]);

				int chSeqID = channel.getChannelSeqID();
				if (chSeqID == -1) continue;
				LOG_INFO << " Seq ID " << chSeqID << ENDL;

				//RICHAlgo prova;
				//int* focalCorrection = prova.getChPosFocalCorr(channel.getDiskID(pmsGeo[chRO[iEdge]]));
				//fitPositionX.push_back(pmsPos[chSeqID*2] - focalCorrection[0]);
				//fitPositionY.push_back(pmsPos[chSeqID*2+1] - focalCorrection[1]);

				LOG_INFO << "chRO " << chRO[iEdge] <<" chGEO " << pmsGeo[chRO[iEdge]] << ENDL;
				LOG_INFO << "X Position " << pmsPos[chSeqID*2] << " Y Position " << pmsPos[chSeqID*2+1] << ENDL;
			}
		}
		//LOG_INFO<< "Number of edges of current board " << numberOfEdgesOfCurrentBoard << ENDL;

		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}

	//LOG_INFO<< " ----- Total Number of edges found -----  " << nEdges_tot << ENDL;

	uint nRings = 0;

	//RICHAlgo algo;
	//LOG_INFO << "X distribution "<< xHitDistribution.at(3) << ENDL;

	return nRings;

}

int* RICHAlgo::getChPosFocalCorr(int diskID) {

	int corrections[2];

	if (diskID == 0) {
		corrections[0] = focalCenterJura[0];
		corrections[1] = focalCenterJura[1];
	} else {
		corrections[0] = focalCenterSaleve[0];
		corrections[1] = focalCenterSaleve[1];
	}
	return corrections;
}

int const RICHAlgo::evaluateXDistribution(vector<double> xHitDistribution) {

	//LOG_INFO << "Test  X distribution method " << xHitDistribution.at(3) << ENDL;
	//auto iter = max_element(xHitDistribution.begin(), xHitDistribution.end());
	//int const max = *iter;
	return 1;
}

}
/* namespace na62 */

