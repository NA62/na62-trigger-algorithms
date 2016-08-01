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

#include "../common/decoding/DecoderHandler.h"
#include "straw_algorithm/ParsConfFile.h"
#include "L1InfoToStorage.h"
#include "../struct/HLTConfParams.h"
#include "L1Fragment.h"
#include "straw_algorithm/Track.h"
#include "straw_algorithm/Point.h"
#include "straw_algorithm/Cluster.h"
#include "straw_algorithm/Straw.h"

#define MAXNHITS 1000
#define MAXNROCHS 512

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
	static void initialize(uint I, l1Straw& L1StrawStruct);
	uint_fast8_t processStrawTrigger(uint L0MaskID, DecoderHandler& Decoder, L1InfoToStorage* L1Info);
	static void writeData(L1StrawAlgo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info);

	static float posTubNew(int Chamber, int View, int Plane, int jStraw);
	static int strawAcceptance(int N, double* Coordinate, int Zone);
	static void cdaVertex(const Point& QBeam, Point& QTrack, const Point& MBeam, Point& MTrack, float& CDA, Point& Vertex);

private:
	static STRAWParsConfFile* InfoSTRAW_;
	static uint AlgoID_; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
	static uint AlgoLogic_[16];
	static uint AlgoRefTimeSourceID_[16];
	static double AlgoOnlineTimeWindow_[16];

	static int* StrawGeo_;
	static double* ROMezzaninesT0_;
	static double StationT0_;

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
	Point strawPointTemp_[4][5000];
	Point strawPointTempbis_[4][2000];
	Point strawPointFinal_[4][2000];
	Cluster strawCluster_[4][4][500];
	Straw strawPrecluster_[4][4][2][500];
	Track strawFirstTempTrk_[3000];
	Track strawTempTrk_[4000];
	Track strawTrkIntermedie_[1000];

	static const double Sq2_;
	static const double InvSq2_;

}
;

} /* namespace na62 */
#endif
