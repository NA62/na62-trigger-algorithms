/*
 * StrawAlgo.h
 *
 *  Created on: Jan 25, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

#pragma once
#ifndef STRAWALGORITHM_H_
#define STRAWALGORITHM_H_

#include <sys/types.h>
#include <cstdint>
#include <mutex>

#ifndef ONLINEHLT
#include "../common/decoding/DecoderHandler.h"
#else
#include "../common/decoding/OfflineDecoderHandler.h"
#endif

#include <l1/L1InfoToStorage.h>
#include "straw_algorithm/ParsConfFile.h"
#include "../struct/HLTConfParams.h"
#include "L1Fragment.h"

#include "straw_algorithm/Track.h"
#include "straw_algorithm/Point.h"
#include "straw_algorithm/Cluster.h"
#include "straw_algorithm/Straw.h"

#define MAX_N_HITS 1500
#define MAX_N_ROCHS 512
#define MAX_N_TRACKS 4000
#define MAX_N_POINT_TEMP 5000
#define MAX_N_POINT_TEMP_BIS 2000
#define MAX_N_POINT_FINAL 2000
#define MAX_N_CLUSTER 500
#define MAX_N_PRECLUSTER MAX_N_CLUSTER // These happen to be the same but one could change it
#define MAX_N_FIRST_TEMP_TRACK 3000
#define MAX_N_TEMP_TRACK 4000
#define MAX_N_TRACK_INTERMEDIE 1000


#define RANGEM 160
#define RANGEQ 200

namespace na62 {

class Event;

class StrawAlgo {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */
	StrawAlgo();
	static void initialize(uint i, l1Straw& l1StrawStruct);
	uint_fast8_t processStrawTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info);
	static uint_fast8_t abortProcessing(L1InfoToStorage* l1Info);
	static void writeData(L1StrawAlgo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info);

	static float posTubNew(int chamber, int view, int plane, int jStraw);
	static int strawAcceptance(int n, double* coordinate, int zone);
	static void cdaVertex(const Point& qBeam, Point& qTrack, const Point& mBeam, Point& mTrack, float& cda, Point& vertex);

private:
	static STRAWParsConfFile* InfoSTRAW_;
	static uint AlgoID_; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
	static uint AlgoLogic_[16];
	static uint AlgoRefTimeSourceID_[16];
	static double AlgoOnlineTimeWindow_[16];

	static int* StrawGeo_;
	static double* ROMezzaninesT0_;
	static double StationT0_;
	static double MagicT0_;

	static const double CutLowLeading_;
	static const double CutHighLeading_;
	static const double CutLowTrailing_;
	static const double CutHighTrailing_;
	static const double M1LeadTrail_;
	static const double Q1LeadTrail_;
	static const double M2LeadTrail_;
	static const double Q2LeadTrail_;
	static const double Hit3Low_;
	static const double Hit3High_;
	static const double Hit2Low_;
	static const double Hit2High_;
	static const int CutCluster_;

	static const float ChamberZPosition_[4];

	static const double XOffset_[4][4];
	// Offset along Z axis, XOffset_[view][plane]
	static const double XOffCh_[4][4];
	// Offset along Z axis, XOffCh_[chamber][view]
	static const float HoleChamberMax_[4][4];
	static const float HoleChamberMin_[4][4];

	static const Point QBeam_;
	static const Point MBeam_;
	Point strawPointTemp_[4][MAX_N_POINT_TEMP];
	Point strawPointTempBis_[4][MAX_N_POINT_TEMP_BIS];
	Point strawPointFinal_[4][MAX_N_POINT_FINAL];
	Cluster strawCluster_[4][4][MAX_N_CLUSTER];
	Straw strawPrecluster_[4][4][2][MAX_N_PRECLUSTER];
	Track strawFirstTempTrk_[MAX_N_FIRST_TEMP_TRACK];
	Track strawTempTrk_[MAX_N_TEMP_TRACK];
	Track strawTrkIntermedie_[MAX_N_TRACK_INTERMEDIE];

	static const double Sq2_;
	static const double InvSq2_;

};

} /* namespace na62 */
#endif
