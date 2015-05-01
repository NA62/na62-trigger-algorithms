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

#include <sys/time.h>

#define ns 1

namespace na62 {

ParsConfFile* RICHAlgo::infoRICH_ = ParsConfFile::GetInstance();
int* RICHAlgo::pmsGeo = infoRICH_->getGeoPmsMap();
double* RICHAlgo::pmsPos = infoRICH_->getPosPmsMap();
int* RICHAlgo::focalCenterJura = infoRICH_->getFocalCenterJura();
int* RICHAlgo::focalCenterSaleve = infoRICH_->getFocalCenterSaleve();
double* RICHAlgo::t0Time = infoRICH_->getT0();
int RICHAlgo::minPMsForEvent = infoRICH_->getMinPMsForEvent();
int RICHAlgo::nHits;
uint RICHAlgo::chRO[maxNhits];
int RICHAlgo::nCandidates;
double RICHAlgo::fitPositionX[maxNhits];
double RICHAlgo::fitPositionY[maxNhits];
double RICHAlgo::leadRecoTime[maxNhits];
int RICHAlgo::focalCorrection[2];

TimeCandidate RICHAlgo::timeCandidates[maxNCands];

uint_fast8_t RICHAlgo::processRICHTrigger(DecoderHandler& decoder) {

	struct timeval time[10];

	gettimeofday(&time[0], 0);
//	gettimeofday(&timeInfo1, 0);
//	LOG_INFO<< "Initial Time" << pippo.tv_usec << ENDL;

	using namespace l0;

//	LOG_INFO<<"####### RICHAlgo.cpp: Analysing Event #### " << decoder.getDecodedEvent()->getEventNumber() << ENDL;

//	infoRICH_ = ParsConfFile::GetInstance();

	nHits = 0;
	nCandidates = 0;
//	pmsGeo = infoRICH_->getGeoPmsMap();
//	pmsPos = infoRICH_->getPosPmsMap();
//	t0Time = infoRICH_->getT0();

//	fitPositionX = new double[maxNhits];
//	fitPositionY = new double[maxNhits];
//	leadRecoTime = new double[maxNhits];

	double* edge_times_ns = new double[maxNhits]; // to be delated, just for time-check!

	gettimeofday(&time[1], 0);

	//LOG_INFO << "Geof CH " << pmsPos[20] << endl;

	uint nEdges_tot = 0;

	for (TrbFragmentDecoder* richPacket : decoder.getRICHDecoderRange()) {

		//LOG_INFO<< "Number of RICH Tel62s = " << decoder.getNumberOfRICHFragments() << ENDL;

//		LOG_INFO<< "RICHAlgo::TEL62 ID = " << richPacket->getFragmentNumber() << ENDL;

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

			edge_times_ns[iEdge + nEdges_tot] = (edge_times[iEdge]
					- decoder.getDecodedEvent()->getTimestamp() * 256.)
					* 0.097464731802;

//			LOG_INFO<<"RichAlgo::ID " << (uint)edge_IDs[iEdge] << ENDL;
//			LOG_INFO<< "RichAlgo::chID " << (uint)edge_chIDs[iEdge] << ENDL;
//			LOG_INFO<<"RichAlgo::tdcID " << (uint)edge_tdcIDs[iEdge] << ENDL;
//			LOG_INFO<< "RichAlgo::time " <<" time [ns]" << edge_times_ns[iEdge]<< ENDL;

//=============== Working with leading time only===================

			if (edge_IDs[iEdge]) {

				chRO[nHits] = edge_trbIDs * 512 + edge_tdcIDs[iEdge] * 32
						+ edge_chIDs[iEdge];

				RICHChannelID channel(pmsGeo[chRO[nHits]]);

				int chSeqID = channel.getChannelSeqID();
				if (chSeqID == -1)
					continue;
//				LOG_INFO << "Seq ID " << chSeqID << ENDL;
				int newSeqID = chSeqID % 976;
				//	LOG_INFO<< "new SeqID " << newSeqID << ENDL;

				//int* focalCorrection = new int[2];
				getChPosFocalCorr(channel.getDiskID(pmsGeo[chRO[nHits]]));

//				LOG_INFO << "Disk ID " << channel.getDiskID(pmsGeo[chRO[nHits]]) << " X correction "<<focalCorrection[0] << ENDL;

				fitPositionX[nHits] = pmsPos[newSeqID * 2] - focalCorrection[0];
				fitPositionY[nHits] = pmsPos[newSeqID * 2 + 1]
						- focalCorrection[1];
				leadRecoTime[nHits] = edge_times_ns[iEdge + nEdges_tot]
						- t0Time[chSeqID];

				//			LOG_INFO<<"iLead " << nHits << " chID " << pmsGeo[chRO[nHits]] << " seq " << chSeqID <<" ChRO " << chRO[nHits]<< "  time " << edge_times_ns[iEdge+nEdges_tot] << ENDL;

				//LOG_INFO << "chRO " << chRO[iEdge] <<" chGEO " << pmsGeo[chRO[iEdge]] << ENDL;
				//LOG_INFO << "T0 time " << t0Time[688] << ENDL;

//				LOG_INFO <<"X position " << pmsPos[newSeqID*2] << " X fit position " << fitPositionX[nHits] << ENDL;
//				LOG_INFO<< "Y position " << pmsPos[newSeqID*2+1] << " Y fit position " << fitPositionY[nHits] << ENDL;
// 				LOG_INFO<<"Index " <<nHits <<" Edge Time[ns] " << edge_times_ns[iEdge + nEdges_tot] << " Rec Time " << leadRecoTime[nHits] << ENDL;

				nHits++;

			}
		}

		//LOG_INFO<< "Number of edges of current board " << numberOfEdgesOfCurrentBoard << ENDL;

		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}
	gettimeofday(&time[2], 0);
//	LOG_INFO << ((timeInfo2.tv_usec + timeInfo2.tv_sec*1e6)-(timeInfo1.tv_usec + timeInfo1.tv_sec*1e6))*0.7 << ENDL;
//	LOG_INFO<< "nHits " << nHits << ENDL;
//	LOG_INFO<< "DeltaX value " << evalDeltaX(fitPositionX) << ENDL;
//	LOG_INFO<< "DeltaY value " << evalDeltaY(fitPositionY) << ENDL;

// ************** 1 RING TO BE STUDIED ***********************
//	double deltaX = evalDeltaX(fitPositionX, nHits);
//	if (nHits < 20 && deltaX < 300) {
//		LOG_INFO<< "1 Ring " << ENDL;
//		return 1;
//	}
	//*************************************************************

	//gettimeofday(&time1, 0);

	if (nHits > 0) {

		gettimeofday(&time[3], 0);
		timeClustering(leadRecoTime);
	}
	gettimeofday(&time[4], 0);
//	if (deltaX >= 300) {
//		TimeCandidate timeCandidates[maxNCands];
//
//		//==========Starting time clustering: Time Candidate=======
//		timeClustering(leadTime, nHits, timeCandidates);
//	}

//	LOG_INFO<< "nHits " << " " << "nCandidates" << ENDL;
//	LOG_INFO<< nHits << "      " << nCandidates << ENDL;

	uint nRings = 0;

//	delete[] fitPositionX;
//	delete[] fitPositionY;
//	delete[] leadRecoTime;

	gettimeofday(&time[5], 0);

	if (nHits > 0) {
		LOG_INFO<< "CIAO ";
		for (int i = 0; i < 5; i++) {
			LOG_INFO << ((time[i+1].tv_usec + time[i+1].tv_sec*1e6)-(time[i].tv_usec + time[i].tv_sec*1e6))*0.7 << " ";
		}
		LOG_INFO <<ENDL;
	}
	return nRings;

}

int* RICHAlgo::getChPosFocalCorr(int diskID) {

//	int* focalCenterJura = infoRICH_->getFocalCenterJura();
//	int* focalCenterSaleve = infoRICH_->getFocalCenterSaleve();

	if (diskID == 0) {
		focalCorrection[0] = focalCenterJura[0];
		focalCorrection[1] = focalCenterJura[1];
	} else {
		focalCorrection[0] = focalCenterSaleve[0];
		focalCorrection[1] = focalCenterSaleve[1];
	}
	return focalCorrection;
}

double RICHAlgo::evalDeltaX(double* fitPositionX) {

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

double RICHAlgo::evalDeltaY(double* fitPositionY) {

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

void RICHAlgo::timeClustering(double* leadRecoTime) {

	struct timeval clusttime1;
	struct timeval clusttime2;

	gettimeofday(&clusttime1, 0);

	nCandidates = 0;
	int nLocalCand;
	int nCandClusteringIterations = 2;
	int nTotEdge[maxNCands];

//	LOG_INFO<< "nHits " << nHits << ENDL;

	for (int iIter = 0; iIter < nCandClusteringIterations; ++iIter) { // loop on iterator for time clustering

		int timeWindow = 10 * ns;

		for (int i = 0; i < maxNCands; ++i) {
			nTotEdge[i] = 0;
		}

		for (int iEdge = 0; iEdge < nHits; iEdge++) {

//			LOG_INFO<<"TimeClustering:: iEdge " << iEdge << ENDL;

			bool flagNewCluster = true;
			double edgeTime = leadRecoTime[iEdge];
			double edgeCandidateTime = 9999 * ns;
			int candidateID = -1;

//			LOG_INFO<<"TimeClustering:: RecoTime " << leadRecoTime[iEdge] << ENDL;

			//LOG_INFO<< "LeadTime Cluster method " << leadRecoTime[iEdge] << ENDL;

			for (int iCand = 0; iCand < nCandidates; iCand++) {

//				LOG_INFO<<"TimeClustering:: iCand " << iCand << " Cand time  " << timeCandidates[iCand].getCandTime()<< ENDL;

				if (fabs(timeCandidates[iCand].getCandTime() - edgeTime)
						< edgeCandidateTime) {
//					LOG_INFO << "deltaTime btw edge " << iEdge<<" and cluster ID " << iCand << " is " << timeCandidates[iCand].getCandTime() - edgeTime << ENDL;
//					LOG_INFO << "edgeCandTime initial " << edgeCandidateTime << ENDL;
					edgeCandidateTime = fabs(
							timeCandidates[iCand].getCandTime() - edgeTime);
					candidateID = iCand;
//					LOG_INFO << "Candidate ID " << candidateID << " edgeCandTime final " << edgeCandidateTime << ENDL;
				}

			}
			if (candidateID >= 0) {

				if (fabs(timeCandidates[candidateID].getCandTime() - edgeTime)
						< 0.5 * timeWindow) {
//					LOG_INFO << "Candidate ID " << candidateID << " delta time " << timeCandidates[candidateID].getCandTime() - edgeTime<< ENDL;
					timeCandidates[candidateID].addEdgeIndexes(iEdge,
							nTotEdge[candidateID]);
//					LOG_INFO << "iEdge " << iEdge<<" CandID " << candidateID<< " Index pos " << nTotEdge[candidateID] <<" Indexes " << timeCandidates[candidateID].getEdgeIndexes()[nTotEdge[candidateID]] << ENDL;
					++nTotEdge[candidateID];
					flagNewCluster = false;
					timeCandidates[candidateID].setNHits(nTotEdge[candidateID]);
//					LOG_INFO << "Candidate ID " << candidateID << " ntothit " << nTotEdge[candidateID]<< " == "<< timeCandidates[candidateID].getNHits()<< endl;

				}
			}

			if (flagNewCluster) {
				candidateID = nCandidates;
//				LOG_INFO << "Flag new cluster ID " << candidateID << ENDL;
				timeCandidates[candidateID].addEdgeIndexes(iEdge,
						nTotEdge[candidateID]);
//				LOG_INFO << "iEdge " << iEdge<< " CandID " << candidateID << " Index pos " << nTotEdge[candidateID] << " Indexes " << timeCandidates[candidateID].getEdgeIndexes()[nTotEdge[candidateID]] << ENDL;
				++nTotEdge[candidateID];
				++nCandidates;
				timeCandidates[candidateID].setNHits(nTotEdge[candidateID]);
//				LOG_INFO << "CandidateID " << candidateID << " ntot hit " << nTotEdge[candidateID]<< " == " << timeCandidates[candidateID].getNHits()<< " total candidate now " << nCandidates<<ENDL;
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

			if (flagNewCluster) {
//				LOG_INFO << "Setting timeCand to NEWCandID " << candidateID << " "<< edgeTime<<ENDL;
				timeCandidates[candidateID].addCandTime(edgeTime);
			} else {
//				LOG_INFO << "Setting final timeCand to CandID " << candidateID << " "<< timeCandidates[candidateID].getCandTime()<<ENDL;
//				LOG_INFO << "Final Hit of CandID " << candidateID << " " << timeCandidates[candidateID].getNHits() << ENDL;
//				LOG_INFO << "Hit Time used to set timeCand " << edgeTime << ENDL;
//				LOG_INFO << "Denominator " << timeCandidates[candidateID].getNHits() << ENDL;

				double updateTime = (timeCandidates[candidateID].getCandTime()
						* (timeCandidates[candidateID].getNHits() - 1)
						+ edgeTime) / timeCandidates[candidateID].getNHits();

//				LOG_INFO << "Nominator " << timeCandidates[candidateID].getCandTime()*(timeCandidates[candidateID].getNHits()-1)+edgeTime<<ENDL;
//				LOG_INFO << "Denominator " << timeCandidates[candidateID].getNHits() << ENDL;
				timeCandidates[candidateID].addCandTime(updateTime);
//				LOG_INFO << "Final Time set " << updateTime << " == " << timeCandidates[candidateID].getCandTime()<< ENDL;
			}

		}
//******FIRST CHECKPOINT*******
//		LOG_INFO<< "END: nTimeCluster " << nCandidates << ENDL;
//		for (int i = 0; i < nCandidates; i++) {
//			int* index = timeCandidates[i].getEdgeIndexes();
//			LOG_INFO<< "iCand " << i << " nHit " << timeCandidates[i].getNHits()<< " time " << timeCandidates[i].getCandTime()<<ENDL;
//			for (int j = 0; j < timeCandidates[i].getNHits(); j++) {
////				LOG_INFO<<"iHit "<< j << " Index  " << index[j]<<"  RecoTime " << leadRecoTime[index[j]] << ENDL;
//
//			}

//		}

		//********calculate Time-Candidates info************

		for (int iCand = 0; iCand < nCandidates; ++iCand) {

			double candidateTime = timeCandidates[iCand].getCandTime();
			if (timeCandidates[iCand].getNHits() >= minPMsForEvent)
				timeCandidates[iCand].setIsSelected(true);
			double deltaTimeClosestCandidate = 999 * ns;
			int nHitsClosestCandidate = 0;

			//Check the minimum distance from any other cluster
			for (int jCand = 0; jCand < nCandidates; ++jCand) {
//				LOG_INFO<< "iCand " << iCand << " iCandTime " << timeCandidates[iCand].getCandTime()<< " jCand " << jCand << " jCandTime " << timeCandidates[jCand].getCandTime() << " deltaTimeCand btw i&&j " << fabs(timeCandidates[jCand].getCandTime()- candidateTime) << ENDL;
				if (iCand != jCand
						&& fabs(
								timeCandidates[jCand].getCandTime()
										- candidateTime)
								< fabs(deltaTimeClosestCandidate)) {
					deltaTimeClosestCandidate =
							timeCandidates[jCand].getCandTime() - candidateTime;
					nHitsClosestCandidate = timeCandidates[jCand].getNHits();
				}

			}
			timeCandidates[iCand].setDeltaTimeClosestCandidate(
					deltaTimeClosestCandidate);
			timeCandidates[iCand].setNHitsClosestCandidate(
					nHitsClosestCandidate);

//			LOG_INFO<< "iIter " << iIter << " iCand " << iCand<<" DeltaTimeClosest " << deltaTimeClosestCandidate << ENDL;
//			LOG_INFO<<"iIter " << iIter << " iCand " << iCand << " nHitsClosest " << nHitsClosestCandidate << ENDL;
		}

		nLocalCand = nCandidates;
//		LOG_INFO<< "NCANDIDATES " << nCandidates<< ENDL;

		for (int iCand = 0; iCand < nCandidates; ++iCand) {
//			LOG_INFO << "iCand " << iCand << ENDL;
			if (iIter < nCandClusteringIterations - 1) {
				if (!timeCandidates[iCand].getIsSelected()
						|| (timeCandidates[iCand].getNHits()
								< timeCandidates[iCand].getNHitsClosestCandidate()
								&& timeCandidates[iCand].getDeltaTimeClosestCandidate()
										< timeWindow)) {
					timeCandidates[iCand].setIsRemoved(true);
					--nLocalCand;
//					LOG_INFO<< "Intermidate nCandidate " << nLocalCand<<"  nCandidates " << nCandidates<<ENDL;

				} else {
					int* indexes = timeCandidates[iCand].getEdgeIndexes();
					for (int iHitCand = 0;
							iHitCand < timeCandidates[iCand].getNHits();
							++iHitCand) {
						double hitTime = leadRecoTime[indexes[iHitCand]];
						if (fabs(hitTime - timeCandidates[iCand].getCandTime())
								> 0.5 * timeWindow) {   /// is it necessary?

							timeCandidates[iCand].removeEdgeIndexes(
									indexes[iHitCand]);
							timeCandidates[iCand].setNHits(
									timeCandidates[iCand].getNHits() - 1);
							timeCandidates[iCand].addCandTime(0.);
							for (int i = 0;
									i < timeCandidates[iCand].getNHits(); ++i) {
								if (indexes[iHitCand] > -1) {
									timeCandidates[iCand].addCandTime(
											timeCandidates[iCand].getCandTime()
													+ leadRecoTime[indexes[iHitCand]]);
								}
							}
							timeCandidates[iCand].addCandTime(
									timeCandidates[iCand].getCandTime()
											/ timeCandidates[iCand].getNHits());
						}
					}
				}
			}

//			LOG_INFO<< "nLocal " << nLocalCand << ENDL;
			//nCandidates = nLocalCand;

//			if (timeCandidates[iCand].getIsRemoved()) {
//				LOG_INFO<< "TimeCand ID " << iCand << " removed " << ENDL;
//			}
//
//			if (!timeCandidates[iCand].getIsRemoved()) {
//				LOG_INFO<< "Time CanID " << iCand << " not removed  nHits " << timeCandidates[iCand].getNHits() << ENDL;
//			}
		}
		nCandidates = nLocalCand;
//		LOG_INFO<<"ENDLOOP: Iter " << iIter << " nCandidates " << nCandidates << ENDL;
//		LOG_INFO << "nCandidates " << nCandidates << ENDL;
	}

	gettimeofday(&clusttime2, 0);
	//LOG_INFO<< ((clusttime2.tv_usec + clusttime2.tv_sec*1e6)-(clusttime1.tv_usec + clusttime1.tv_sec*1e6))*0.7 << ENDL;

}

}
/* namespace na62 */

