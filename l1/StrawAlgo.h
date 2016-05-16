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

#include "../common/decoding/DecoderHandler.h"
#include "straw_algorithm/ParsConfFile.h"
#include "L1InfoToStorage.h"

#define MAXNHITS 1000
#define MAXNROCHS 512

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
	static uint_fast8_t processStrawTrigger(DecoderHandler& decoder, L1InfoToStorage* l1Info);
	static float posTubNew(int chamber, int view, int plane, int jstraw);
	static int strawAcceptance(int n, double *coor, int zone);
	static int cdaVertex(Point qfascio, Point qtraccia, Point mfascio,
				Point mtraccia, float* cda, Point* vertice);

	private:
		static STRAWParsConfFile* infoSTRAW_;
		static int* strawGeo;
		static double* fROMezzaninesT0;
		static uint chRO[MAXNHITS];

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

		static bool tl_flag;
		static bool skip_flag;
		static int nHits;

		static int chamberID;
		static int viewID;
		static int halfviewID;
		static int planeID;
		static int strawID;
		static double leading;
		static double trailing;
		static float position;
		static float wireDistance;

		static STRAWChannelID strawChannel_;
		static DigiManager strawDigiMan_;
		static float fChamberZPosition[4];
		static Straw strawPrecluster_[4][4][2][500];
		static int nStrawPreclusters[4][4][2];
		static Cluster strawCluster_[4][4][500];
		static int nStrawClusters[4][4];
		static Point strawPointTemp_[4][5000];
		static int nStrawPointsTemp[4];
		static Point strawPointTempbis_[4][2000];
		static int nStrawPointsTempbis[4];
		static Point strawPointFinal_[4][2000];
		static int nStrawPointsFinal[4];
		static Track strawFirstTempTrk_[3000];
		static Track strawTempTrk_[4000];
		static Track strawTrkIntermedie_[1000];
	}
	;

} /* namespace na62 */
#endif
