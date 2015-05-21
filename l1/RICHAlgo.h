/*
 * RICHAlgo.h
 *
 *  Created on: 17 Apr 2015
 *      Author: Valeria Fascianelli
 */
#pragma once
#ifndef L1TRIGGERPROCESSOR_RICHALGO_H_
#define L1TRIGGERPROCESSOR_RICHALGO_H_

#include <sys/types.h>
#include <cstdint>
#include <map>
#include <vector>

#include "../common/decoding/DecoderHandler.h"
#include "rich_algorithm/TimeCandidate.h"
#include "rich_algorithm/ParsConfFile.h"

#define maxNhits 500
#define maxNCands 100

namespace na62 {

class Event;

class RICHAlgo {
public:

	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	static uint_fast8_t processRICHTrigger(DecoderHandler& decoder);
	static double evalDeltaX();
	static double evalDeltaY();
	static int* getChPosFocalCorr(int diskID);
	static void timeClustering();
	static void mapping();
	static void multiRingReco();
	static bool startingTriplet(int iSide);
	static void monoRingFit();


private:
    static uint chRO[maxNhits];
	static int nHits;
	static RICHParsConfFile* infoRICH_;
	static double* t0Time;
	static int* pmsGeo;
	static double* pmsPos;
	static int* focalCenterJura;
	static int* focalCenterSaleve;
	static int minPMsForEvent;
	static int nCandClusteringIterations;
	static int nCandidates;
	static TimeCandidate timeCandidates[maxNCands];
	static double fitPositionX[maxNhits];
	static double fitPositionY[maxNhits];
	static double leadRecoTime[maxNhits];
	static int focalCorrection[2];
	static double edge_times_ns[maxNhits];
	static int timeWindow;
	//static vector<pair<int, double>> sortMapX;
	static pair<int, double> pairX;
	//typedef static pair<int, double> sortPair;
	static float centerRing[2];
	static float radiusRing;



};

} /* namespace na62 */

#endif /* L1TRIGGERPROCESSOR_RICHALGO_H_ */
