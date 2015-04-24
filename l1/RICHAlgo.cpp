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
	int nHits = 0;
	int* pmsGeo = mapsChs->getGeoPmsMap();
	double* pmsPos = mapsChs->getPosPmsMap();

	double* fitPositionX = new double[maxNhits];
	double* fitPositionY = new double[maxNhits];

	//LOG_INFO << "Geof CH " << pmsPos[20] << endl;

	uint nEdges_tot = 0;
	uint chRO[maxNhits];

	for (TrbFragmentDecoder* richPacket : decoder.getRICHDecoderRange()) {

		//LOG_INFO<< "Number of RICH Tel62s = " << decoder.getNumberOfRICHFragments() << ENDL;

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

				chRO[nHits] = edge_trbIDs * 512 + edge_tdcIDs[iEdge] * 32
				+ edge_chIDs[iEdge];

				RICHChannelID channel(pmsGeo[chRO[nHits]]);

				int chSeqID = channel.getChannelSeqID();
				if (chSeqID == -1) continue;
				LOG_INFO << " Seq ID " << chSeqID << ENDL;
				int newSeqID = chSeqID%976;
				LOG_INFO<< "new SeqID " << newSeqID << ENDL;

				int* focalCorrection = new int[2];
				getChPosFocalCorr(channel.getDiskID(pmsGeo[chRO[nHits]]), focalCorrection);

				LOG_INFO << "Disk ID " << channel.getDiskID(pmsGeo[chRO[nHits]]) << " X correction "<<focalCorrection[0] << ENDL;

				fitPositionX[nHits] = pmsPos[newSeqID*2] - focalCorrection[0];
				fitPositionY[nHits] = pmsPos[newSeqID*2+1] - focalCorrection[1];

				//LOG_INFO << "chRO " << chRO[iEdge] <<" chGEO " << pmsGeo[chRO[iEdge]] << ENDL;

				LOG_INFO << "X Position " << pmsPos[newSeqID*2] << " X fit Position " << fitPositionX[nHits] << ENDL;
				LOG_INFO<< " Y position " << pmsPos[newSeqID*2+1] << " Y fit Position " << fitPositionY[nHits] << ENDL;

				nHits++;

			}
		}
		//LOG_INFO<< "Number of edges of current board " << numberOfEdgesOfCurrentBoard << ENDL;

		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}

	LOG_INFO<< "NHits " << nHits << ENDL;
	cout << " DeltaX value " << evalDeltaX(fitPositionX, nHits) << ENDL;
	cout << " DeltaY value " << evalDeltaY(fitPositionY, nHits) << ENDL;


	// ************** 1 RING TO BE STUDIED ***********************
	double deltaX = evalDeltaX(fitPositionX, nHits);
	if (nHits < 20 && deltaX < 300) {
		LOG_INFO<< "1 Ring " << ENDL;
		return 1;
	}
	//*************************************************************



	uint nRings = 0;

	return nRings;

}

int* RICHAlgo::getChPosFocalCorr(int diskID, int* focalCorrection) {

	ParsConfFile* mapsChs = ParsConfFile::GetInstance();

	int* focalCenterJura = mapsChs->getFocalCenterJura();
	int* focalCenterSaleve = mapsChs->getFocalCenterSaleve();

	if (diskID == 0) {
		focalCorrection[0] = focalCenterJura[0];
		focalCorrection[1] = focalCenterJura[1];
	} else {
		focalCorrection[0] = focalCenterSaleve[0];
		focalCorrection[1] = focalCenterSaleve[1];
	}
	return focalCorrection;
}

double RICHAlgo::evalDeltaX(double* fitPositionX, int nHits) {

	double max = fitPositionX[0];
	double min = fitPositionX[0];

	for (int i = 0; i < nHits; i++) {
		if (fitPositionX[i] > max) {
			max = fitPositionX[i];
		}

		if (fitPositionX[i] < min) {
			min = fitPositionX[i];
		}
	}
	return (max - min);
}

double RICHAlgo::evalDeltaY(double* fitPositionY, int nHits) {

	double max = fitPositionY[0];
	double min = fitPositionY[0];

	for (int i = 0; i < nHits; i++) {
		if (fitPositionY[i] > max) {
			max = fitPositionY[i];
		}
		if (fitPositionY[i] < min) {
			min = fitPositionY[i];
		}
	}

	return (max - min);
}
}
/* namespace na62 */

