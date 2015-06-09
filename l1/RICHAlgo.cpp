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
#include <map>

#include "../common/decoding/DecoderRange.h"
#include "../common/decoding/DecoderHandler.h"
#include "../common/decoding/TrbFragmentDecoder.h"

#include "rich_algorithm/ParsConfFile.h"
#include "rich_algorithm/RICHChannelID.h"
#include "rich_algorithm/TimeCandidate.h"

#include <sys/time.h>

#define ns 1

namespace na62 {

RICHParsConfFile* RICHAlgo::infoRICH_ = RICHParsConfFile::GetInstance();
int* RICHAlgo::pmsGeo = infoRICH_->getGeoPmsMap();
double* RICHAlgo::pmsPos = infoRICH_->getPosPmsMap();
int* RICHAlgo::focalCenterJura = infoRICH_->getFocalCenterJura();
int* RICHAlgo::focalCenterSaleve = infoRICH_->getFocalCenterSaleve();
double* RICHAlgo::t0Time = infoRICH_->getT0();
int RICHAlgo::minPMsForEvent = infoRICH_->getMinPMsForEvent();
int RICHAlgo::nCandClusteringIterations =
		infoRICH_->getNCandClusteringIterations();
int RICHAlgo::nHits;
uint RICHAlgo::chRO[maxNhits];
int RICHAlgo::nCandidates;
TimeCandidate RICHAlgo::timeCandidates[maxNCands];
double RICHAlgo::fitPositionX[maxNhits];
double RICHAlgo::fitPositionY[maxNhits];
double RICHAlgo::leadRecoTime[maxNhits];
double RICHAlgo::edge_times_ns[maxNhits];
int RICHAlgo::focalCorrection[2];
int RICHAlgo::timeWindow = 10 * ns;
//vector<pair<int, double>> RICHAlgo::sortMapX;
pair<int, double> RICHAlgo::pairX;
float RICHAlgo::centerRing[2];
float RICHAlgo::radiusRing;

uint_fast8_t RICHAlgo::processRICHTrigger(DecoderHandler& decoder) {

	struct timeval time[10];

	//gettimeofday(&time[0], 0);

	using namespace l0;

//	LOG_INFO<<"####### RICHAlgo.cpp: Analysing Event #### " << decoder.getDecodedEvent()->getEventNumber() << ENDL;

	nHits = 0;
	nCandidates = 0;

	//LOG_INFO << "Geof CH " << pmsPos[20] << endl;

	uint nEdges_tot = 0;

	for (TrbFragmentDecoder* richPacket : decoder.getRICHDecoderRange()) {

		//LOG_INFO<< "Number of RICH Tel62s = " << decoder.getNumberOfRICHFragments() << ENDL;

//		LOG_INFO<< "RICHAlgo::TEL62 ID = " << richPacket->getFragmentNumber() << ENDL;

		/**
		 * Get Arrays with hit Info
		 */

//		if (richPacket->getFragmentNumber() == 4)
//			continue;
		const uint64_t* const edge_times = richPacket->getTimes();

		const uint_fast8_t* const edge_chIDs = richPacket->getChIDs();
		const bool* const edge_IDs = richPacket->getIsLeadings();
		const uint_fast8_t* const edge_tdcIDs = richPacket->getTdcIDs();
		const uint_fast16_t edge_trbIDs = richPacket->getFragmentNumber();

		uint numberOfEdgesOfCurrentBoard = richPacket->getNumberOfEdgesStored();

		//gettimeofday(&time[1], 0);

		const uint_fast8_t fineTime = decoder.getDecodedEvent()->getFinetime();
		double fineTime_ns = 0;

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; ++iEdge) {

			edge_times_ns[iEdge + nEdges_tot] = (edge_times[iEdge]
					- decoder.getDecodedEvent()->getTimestamp() * 256.)
					* 0.097464731802;
			fineTime_ns = (fineTime
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
				int newSeqID = chSeqID % 976;

				if (fabs(edge_times_ns[iEdge + nEdges_tot] - fineTime_ns)
						< 5 * ns || 1) {

//				if (fabs(edge_times_ns[iEdge + nEdges_tot] - fineTime_ns) < 5) {

					getChPosFocalCorr(channel.getDiskID(pmsGeo[chRO[nHits]]));

					fitPositionX[nHits] = pmsPos[newSeqID * 2]
							- focalCorrection[0];
					fitPositionY[nHits] = pmsPos[newSeqID * 2 + 1]
							- focalCorrection[1];
					leadRecoTime[nHits] = edge_times_ns[iEdge + nEdges_tot]
							- t0Time[chSeqID];


					nHits++;
				}
			}
		}

		//LOG_INFO<< "Number of edges of current board " << numberOfEdgesOfCurrentBoard << ENDL;

		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}

	//LOG_INFO<<"RICHAlgo.cpp: Analysed Event " << decoder.getDecodedEvent()->getEventNumber() << " - nHits " << nHits << ENDL;
	//gettimeofday(&time[1], 0);

//	LOG_INFO<< "nHits " << nHits << ENDL;
//	LOG_INFO<< "DeltaX value " << evalDeltaX(fitPositionX) << ENDL;
//	LOG_INFO<< "DeltaY value " << evalDeltaY(fitPositionY) << ENDL;

	double DeltaX = 0;
	double DeltaY = 0;

	if (nHits > 3) {
		//gettimeofday(&time[3], 0);
		//timeClustering();
		//gettimeofday(&time[2], 0);
		DeltaX = evalDeltaX();
		//gettimeofday(&time[3], 0);
		DeltaY = evalDeltaY();
		//gettimeofday(&time[4], 0);
		monoRingFit();
		//gettimeofday(&time[5], 0);
		//LOG_INFO << "NHits " << nHits << " DeltaX " << DeltaX << " DeltaY " << DeltaY << ENDL;
		//LOG_INFO << "Ring radius " << radiusRing << " center_x " << centerRing[0] << " center_y " << centerRing[1] << ENDL;
	}

	//gettimeofday(&time[5], 0);

	//gettimeofday(&time[6], 0);


//uint nRings = 0;

//	gettimeofday(&time[7], 0);

//	LOG_INFO<< ((time[7].tv_sec - time[0].tv_sec)*1e6 + time[7].tv_usec) - time[0].tv_usec << ENDL;

//	if (nHits > 0) {
//		for (int i = 0; i < 7; i++) {
//			LOG_INFO<< ((time[i+1].tv_usec + time[i+1].tv_sec*1e6)-(time[i].tv_usec + time[i].tv_sec*1e6))*0.7 << " ";
//		}
//		LOG_INFO <<ENDL;
//	}
//	return (nHits > 40 && (DeltaX > 400 || DeltaY > 390));
	return ((nHits > 3) && (nHits < 22) && (DeltaX <= 400 && DeltaY <= 400));

//return nRings;

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

double RICHAlgo::evalDeltaX() {

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

double RICHAlgo::evalDeltaY() {

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

void RICHAlgo::timeClustering() {

	nCandidates = 0;
	int nLocalCand;
	int nTotEdge[maxNCands];

//	LOG_INFO<< "nHits " << nHits << ENDL;

	for (int iIter = 0; iIter < nCandClusteringIterations; ++iIter) { // loop on iterator for time clustering

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
////			LOG_INFO<< "iCand " << i << " nHit " << timeCandidates[i].getNHits()<< " time " << timeCandidates[i].getCandTime()<<ENDL;
//			for (int j = 0; j < timeCandidates[i].getNHits(); j++) {
//				LOG_INFO<<"iHit "<< j << " Index  " << index[j]<<"  RecoTime " << leadRecoTime[index[j]] << ENDL;
//
//			}
//
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

}

void RICHAlgo::multiRingReco() {

}

bool RICHAlgo::startingTriplet(int iSide) {

	return false;
}

bool comparator(const pair<int, double> &a, const pair<int, double> &b) {
	return (a.second < b.second);
}

void RICHAlgo::mapping() {

	struct timeval time[10];

	for (int iCand = 0; iCand < nCandidates; ++iCand) {

		gettimeofday(&time[0], 0);

		//LOG_INFO <<"pointer "<< timeCandidates[iCand].sortMapX << ENDL;

		timeCandidates[iCand].sortMapX.clear();
		//sortMapX.clear();

		gettimeofday(&time[1], 0);

		int* index = timeCandidates[iCand].getEdgeIndexes();

		for (int iHit = 0; iHit < timeCandidates[iCand].getNHits(); ++iHit) {

//			LOG_INFO<< "X index " << index[iHit] << " X value " << fitPositionX[index[iHit]] << ENDL;

			pairX.first = index[iHit];
			pairX.second = fitPositionX[index[iHit]];
			//sortMapX.push_back(pairX);
			timeCandidates[iCand].sortMapX.push_back(pairX);

		}

		gettimeofday(&time[2], 0);
		//sort(sortMapX.begin(), sortMapX.end(), comparator);
		sort(timeCandidates[iCand].sortMapX.begin(),
				timeCandidates[iCand].sortMapX.end(), comparator);
		gettimeofday(&time[3], 0);
		//timeCandidates[iCand].setSortMapX(sortMapX);
		//gettimeofday(&time[4], 0);

		//LOG_INFO <<"pointer "<< timeCandidates[iCand].sortMapX << ENDL;

//		LOG_INFO<<"CIAO ";
//		for (int i = 0; i < 3; i++) {
//			LOG_INFO<< ((time[i+1].tv_usec + time[i+1].tv_sec*1e6)-(time[i].tv_usec + time[i].tv_sec*1e6)) << " ";
//		}
//		LOG_INFO<<ENDL;

//		for (int i = 0; i < timeCandidates[iCand].getNHits(); i++) {
//
//			LOG_INFO<<"X index sorted " << timeCandidates[iCand].sortMapX.at(i).first << " X value " << timeCandidates[iCand].sortMapX.at(i).second << ENDL;
//		}
	}
}

void RICHAlgo::monoRingFit() {

	float xm = 0;
	float ym = 0;
	float xav;
	float yav;
	float newPosX[nHits];
	float newPosY[nHits];
	float Suu = 0;
	float Svv = 0;
	float Suuu = 0;
	float Svvv = 0;
	float Suv = 0;
	float Suvv = 0;
	float Suuv = 0;
	float ui, vi;

	for (int i = 0; i < nHits; ++i) {

		//LOG_INFO<< "fitPos " << fitPositionX[i] << ENDL;
		xm = xm + fitPositionX[i];
		ym = ym + fitPositionY[i];
	}

	xav = xm / nHits;
	yav = ym / nHits;

//	LOG_INFO<< "xSum " << xm << " xMean " << xav << ENDL;

	for (int i = 0; i < nHits; ++i) {

		newPosX[i] = fitPositionX[i] - xav;
		newPosY[i] = fitPositionY[i] - yav;
		//LOG_INFO<< "newPosX " << newPosX[i] << " newPosY " << newPosY[i]<< ENDL;
		ui = newPosX[i];
		vi = newPosY[i];

		//LOG_INFO<< " ui " << ui << " uuu " << ui*ui*ui<< ENDL;

		Suu = Suu + ui * ui;
		Svv = Svv + vi * vi;
		Suv = Suv + ui * vi;
		Suuu = Suuu + ui * ui * ui;
		Svvv = Svvv + vi * vi * vi;
		Suuv = Suuv + ui * ui * vi;
		Suvv = Suvv + vi * vi * ui;

	}

	float cc1 = 0.5 * (Suuu + Suvv);
	float cc2 = 0.5 * (Svvv + Suuv);
	float uc = ((Suv * cc2 - Svv * cc1) / (Suv * Suv - Suu * Svv));
	float vc = (cc1 - Suu * uc) / Suv;
	float alpha = uc * uc + vc * vc + (Suu + Svv) / nHits;

	//LOG_INFO<< " Suv * Suv - Suu * Svv " << Suv * Suv - Suu * Svv << ENDL;
	//LOG_INFO<< "Suu " << Suu <<" Svv " << Svv << " Suv " << Suv << " Suuu " << Suuu << " Svv " << Svvv << " Suuv " << Suuv << " Suvv " << Suvv << ENDL;
	//LOG_INFO<< "cc1 " << cc1 << " cc2 " << cc2 << " uc " << uc << " vc " << vc << " alpha " << alpha<< ENDL;
// compute radius and center

	centerRing[0] = uc + xav;
	centerRing[1] = vc + yav;
	radiusRing = sqrt(alpha);

}

}
/* namespace na62 */

