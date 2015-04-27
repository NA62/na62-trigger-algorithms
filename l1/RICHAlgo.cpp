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
#include <cmath>

#include "../common/decoding/DecoderRange.h"
#include "../common/decoding/DecoderHandler.h"
#include "../common/decoding/TrbFragmentDecoder.h"

#include "rich_algorithm/ParsConfFile.h"
#include "rich_algorithm/RICHChannelID.h"
#include "rich_algorithm/TimeCandidate.h"

#define maxNhits 500
#define maxNCands 10
#define ns 1E-9

namespace na62 {

uint_fast8_t RICHAlgo::processRICHTrigger(DecoderHandler& decoder) {

	using namespace l0;

	LOG_INFO<<"===== RICHAlgo.cpp: Analysing Event === " << decoder.getDecodedEvent()->getEventNumber() << ENDL;

	ParsConfFile* infoRICH_ = ParsConfFile::GetInstance();

	int nHits = 0;
	int* pmsGeo = infoRICH_->getGeoPmsMap();
	double* pmsPos = infoRICH_->getPosPmsMap();
	infoRICH_->readT0();
	LOG_INFO << "time T0 " << infoRICH_->timeT0[20] << ENDL;

	//LOG_INFO << "T0 file " << infoRICH->fileT0<<ENDL;

	double* fitPositionX = new double[maxNhits];
	double* fitPositionY = new double[maxNhits];

	uint64_t* leadTime = new uint64_t[maxNhits];

	double* edge_times_ns = new double[maxNhits]; // to be delated, just for time-check!

	//LOG_INFO << "Geof CH " << pmsPos[20] << endl;

	uint nEdges_tot = 0;
	uint chRO[maxNhits];

	for (TrbFragmentDecoder* richPacket : decoder.getRICHDecoderRange()) {

		//LOG_INFO<< "Number of RICH Tel62s = " << decoder.getNumberOfRICHFragments() << ENDL;

		LOG_INFO<< "RICHAlgo::TEL62 ID = " << richPacket->getFragmentNumber() << ENDL;

		/**
		 * Get Arrays with hit Info
		 */

		const uint64_t* const edge_times = richPacket->getTimes();

		const uint_fast8_t* const edge_chIDs = richPacket->getChIDs();
		const bool* const edge_IDs = richPacket->getIsLeadings();
		const uint_fast8_t* const edge_tdcIDs = richPacket->getTdcIDs();
		const uint_fast16_t edge_trbIDs = richPacket->getFragmentNumber();

		uint numberOfEdgesOfCurrentBoard = richPacket->getNumberOfEdgesStored();

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; ++iEdge) {

			edge_times_ns[iEdge] = (edge_times[iEdge] - decoder.getDecodedEvent()->getTimestamp()*256.)*0.097464731802;

			LOG_INFO<<"RichAlgo::ID " << (uint)edge_IDs[iEdge] << ENDL;
			LOG_INFO<< "RichAlgo::chID " << (uint)edge_chIDs[iEdge] << ENDL;
			LOG_INFO<<"RichAlgo::tdcID " << (uint)edge_tdcIDs[iEdge] << ENDL;
			LOG_INFO<< "RichAlgo::time " << std::hex << edge_times[iEdge] << std::dec <<" time [ns]" << edge_times_ns[iEdge]<< ENDL;

//=============== Working with leading time only===================

			if (edge_IDs[iEdge]) {

				leadTime[nHits] = edge_times_ns[iEdge]; // choose edge_times! ns is just for check!

				//		LOG_INFO <<"Lead " << leadTime[nHits] << " times "<< edge_times[iEdge] << ENDL;
				chRO[nHits] = edge_trbIDs * 512 + edge_tdcIDs[iEdge] * 32
				+ edge_chIDs[iEdge];

				RICHChannelID channel(pmsGeo[chRO[nHits]]);

				int chSeqID = channel.getChannelSeqID();
				if (chSeqID == -1) continue;
				//	LOG_INFO << "Seq ID " << chSeqID << ENDL;
				int newSeqID = chSeqID%976;
				//	LOG_INFO<< "new SeqID " << newSeqID << ENDL;

				int* focalCorrection = new int[2];
				getChPosFocalCorr(channel.getDiskID(pmsGeo[chRO[nHits]]), focalCorrection);

//				LOG_INFO << "Disk ID " << channel.getDiskID(pmsGeo[chRO[nHits]]) << " X correction "<<focalCorrection[0] << ENDL;

				fitPositionX[nHits] = pmsPos[newSeqID*2] - focalCorrection[0];
				fitPositionY[nHits] = pmsPos[newSeqID*2+1] - focalCorrection[1];

//LOG_INFO << "chRO " << chRO[iEdge] <<" chGEO " << pmsGeo[chRO[iEdge]] << ENDL;

//LOG_INFO << "X Position " << pmsPos[newSeqID*2] << " X fit Position " << fitPositionX[nHits] << ENDL;
//LOG_INFO<< "Y position " << pmsPos[newSeqID*2+1] << " Y fit Position " << fitPositionY[nHits] << ENDL;
//LOG_INFO<< " Leading Time " << leadTime[nHits] << ENDL;

				nHits++;

			}
		}
		//LOG_INFO<< "Number of edges of current board " << numberOfEdgesOfCurrentBoard << ENDL;

		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}

//	LOG_INFO<< "NHits " << nHits << ENDL;
//	LOG_INFO << "DeltaX value " << evalDeltaX(fitPositionX, nHits) << ENDL;
//	LOG_INFO<< "DeltaY value " << evalDeltaY(fitPositionY, nHits) << ENDL;

// ************** 1 RING TO BE STUDIED ***********************
//	double deltaX = evalDeltaX(fitPositionX, nHits);
//	if (nHits < 20 && deltaX < 300) {
//		LOG_INFO<< "1 Ring " << ENDL;
//		return 1;
//	}
	//*************************************************************

	TimeCandidate timeCandidates[maxNCands];
	//timeClustering(leadTime, nHits, timeCandidates);

//	if (deltaX >= 300) {
//		TimeCandidate timeCandidates[maxNCands];
//
//		//==========Starting time clustering: Time Candidate=======
//		timeClustering(leadTime, nHits, timeCandidates);
//	}

	uint nRings = 0;

	delete[] fitPositionX;
	delete[] fitPositionY;
	delete[] leadTime;

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
	return fabs(max - min);
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

	return fabs(max - min);
}

void RICHAlgo::timeClustering(double* leadTime, int nHits,
		TimeCandidate* timeCandidates) {

	int nCandidates = 0;
	int nCandClusteringIterations = 2;

	for (int iIter = 0; iIter < nCandClusteringIterations; ++iIter) { // loop on iterator for time clustering

		for (int iEdge = 0; iEdge < nHits; iEdge++) {

			bool flagNewCluster = true;
			double edgeTime = leadTime[iEdge];
			double edgeCandidateTime = 9999 * ns;
			int candidateID = -1;
			int timeWindow = 10 * ns;

			for (int iCand = 0; iCand < nCandidates; iCand++) {

				if (fabs(timeCandidates[iCand].getCandTime() - edgeTime)
						< edgeCandidateTime) {
					edgeCandidateTime = fabs(
							timeCandidates[iCand].getCandTime() - edgeTime);
					candidateID = iCand;
				}
			}
			if (candidateID >= 0) {

				if (fabs(timeCandidates[candidateID].getCandTime() - edgeTime)
						< 0.5 * timeWindow)
					flagNewCluster = false;
			}

			if (flagNewCluster) {
				candidateID = nCandidates;
				++nCandidates;
				//timeCandidates[nCandidates].addCandTime(edgeTime);
			}

			bool flagMissingHit = true;
			if (iIter > 0) {
				int* indexes = timeCandidates[candidateID].getEdgeIndexes();
				for (int i = 0; i < maxNhits / 2; ++i) {
					if (indexes[i] > -1)
						if (indexes[i] == iEdge)
							flagMissingHit = false;
				}
			}

			if (!flagMissingHit)
				continue;

			if (flagNewCluster)
				timeCandidates[candidateID].addCandTime(edgeTime);
			else {
				edgeTime = (timeCandidates[candidateID].getCandTime()
						* (timeCandidates[candidateID].getNHits() - 1)
						/ timeCandidates[candidateID].getNHits());
				timeCandidates[candidateID].addCandTime(edgeTime);
			}
			//LOG_INFO<<"nCandidates " << nCandidates << " time " << timeCandidates[candidateID].getCandTime() << ENDL;
		} // end loop of hits for time clustering

		//calculate time candidates info

	}
}





}
/* namespace na62 */

