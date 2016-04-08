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

#define MAXNHITS 1000
#define MAXNROCHS 512

namespace na62 {

class Event;
class Straw;
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
	static uint_fast8_t processStrawTrigger(DecoderHandler& decoder);
	static float posTubNew(int chamber, int view, int plane, int jstraw);

private:
	static STRAWParsConfFile* infoSTRAW_;
	static int* strawGeo;
	static double* fROMezzaninesT0;
	static uint chRO[MAXNHITS];
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
	static bool tl_flag;
	static STRAWChannelID strawChannel_;
	static DigiManager strawDigiMan_;
	static Straw strawPrecluster_[4][4][2][500];
	static int nStrawPrecluster[4][4][2];
};

} /* namespace na62 */
#endif
