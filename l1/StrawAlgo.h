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

#define MAXNHITS 1000
#define MAXNROCHS 512

#define rangem 160
#define rangeq 200

namespace na62 {

class Event;
class Straw;
class Cluster;
class Point;
class Track;
class STRAWChannelID;
class DigiManager;

class StrawAlgo {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */
	static void initialize(uint i, l1Straw &l1StrawStruct);
	static uint_fast8_t processStrawTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info);
	static void writeData(L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info);

	static float posTubNew(int chamber, int view, int plane, int jstraw);
	static int strawAcceptance(int n, double *coor, int zone);
	static int cdaVertex(Point qfascio, Point qtraccia, Point mfascio, Point mtraccia, float* cda, Point* vertice);

private:
	static STRAWParsConfFile* InfoSTRAW_;
	static uint AlgoID_; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
	static uint AlgoLogic_[16];
	static uint AlgoRefTimeSourceID_[16];
	static double AlgoOnlineTimeWindow_[16];

	static int* StrawGeo_;
	static double* ROMezzaninesT0_;

	static double t0_main_shift;
	static double cutlowleading;
	static double cuthighleading;
	static double cutlowtrailing;
	static double cuthightrailing;
	static double m1leadtrail;
	static double q1leadtrail;
	static double m2leadtrail;
	static double q2leadtrail;
	static double hit3low;
	static double hit3high;
	static double hit2low;
	static double hit2high;
	static int cutcluster;

	static float fChamberZPosition[4];

	static Point qbeam;
	static Point mbeam;
	static Point strawPointTemp_[4][5000];
	static Point strawPointTempbis_[4][2000];
	static Point strawPointFinal_[4][2000];
	static Cluster strawCluster_[4][4][500];
	static Straw strawPrecluster_[4][4][2][500];
	static Track strawFirstTempTrk_[3000];
	static Track strawTempTrk_[4000];
	static Track strawTrkIntermedie_[1000];

	static std::mutex strawMutex;

}
;

} /* namespace na62 */
#endif
