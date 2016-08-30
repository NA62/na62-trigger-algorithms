/*
 * SrbDecoder.cpp
 *
 *  Created on: Nov 24, 2014
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

#include "SrbFragmentDecoder.h"

#include <stdlib.h>
#include <cstdint>
#include <iomanip>
#include <options/Logging.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <iostream>
#include <fstream>

#define ClockPeriod 24.951059536

namespace na62 {

SrbFragmentDecoder::SrbFragmentDecoder() :
		edgeTimes(nullptr), edgeStrawIDs(nullptr), edgeSrbIDs(nullptr), edgeErrorFlags(nullptr), edgeIsLeading(nullptr), subevent_(nullptr), fragmentNumber_(
		UINT_FAST16_MAX) {
	firstTSCoarseTime = 0;
	nEdges_tot = 0;
	isBadFrag_ = false;
}

SrbFragmentDecoder::~SrbFragmentDecoder() {
	if (isReady()) {
		delete[] edgeTimes;
		delete[] edgeErrorFlags;
		delete[] edgeStrawIDs;
		delete[] edgeIsLeading;
		delete[] edgeSrbIDs;
	}
}

void SrbFragmentDecoder::readData(uint_fast32_t timestamp) {
	// Don't run again if we already read the data
	if (edgeTimes != nullptr) {
		return;
	}

	const l0::MEPFragment* const srbDataFragment = subevent_->getFragment(fragmentNumber_);

	//	LOG_INFO("srbData " << srbDataFragment->getPayloadLength());
	if (!srbDataFragment->getPayloadLength()) {
		isBadFrag_ = true;
		return;
	}
	/*
	 * Each data word is 2 bytes. Each header word is 4 bytes.
	 * there is a 2 word board header
	 * there are 4 slot words
	 * -> use this to estimate the maximum number of datawords
	 * there are 2 edges in a dataword
	 * -> use this to estimate the maximum number of edges
	 */
	const uint maxNwords = (srbDataFragment->getPayloadLength() / 4);

	if (maxNwords <= 0) {
		LOG_ERROR("The packet payload is not as expected !!!");
		//throw NA62Error("The packet payload is not as expected !!!");
		isBadFrag_ = true;
		return;
	}

	const uint maxNEdges = maxNwords * 2;

	//LOG_INFO("srbData " << srbDataFragment->getPayloadLength());
	//LOG_INFO("maxNEdges = " << maxNEdges);
	edgeTimes = new double[maxNEdges];
	edgeStrawIDs = new uint_fast8_t[maxNEdges];
	edgeErrorFlags = new uint_fast8_t[maxNEdges];
	edgeIsLeading = new bool[maxNEdges];
	edgeSrbIDs = new uint_fast8_t[maxNEdges];

	//initialize
	for (uint i = 0; i < maxNwords; i++) {
		edgeTimes[i] = 0;
		edgeStrawIDs[i] = 0;
		edgeErrorFlags[i] = 0;
		edgeIsLeading[i] = 0;
		edgeSrbIDs[i] = 0;
	}

	const char* const payload = srbDataFragment->getPayload();

	const SrbDataHeader* const boardHeader = reinterpret_cast<const SrbDataHeader*>(payload);

	firstTSCoarseTime = (int64_t) boardHeader->firstTSCoarseTime;
	uint_fast8_t SrbID = boardHeader->srbID;
	//LOG_INFO("SRBid = " << SrbID);
	//LOG_INFO("Event Timestamp " << std::hex << timestamp << std::dec);
	//LOG_INFO("firstTSCoarseTime " << std::hex << firstTSCoarseTime << std::dec);

	firstTSCoarseTime -= timestamp;

	//number of datawords, computed from packet length (in bytes)
	//2 header words and 4 slot words subtracted
	const uint16_t nWords = ((uint16_t) boardHeader->packetLength) / 4 - 2 - 4;
	const uint16_t nEdges = nWords * 2; //+1 empty edge if nWords in raw file was odd

	//if (! nEdges)
	//LOG_INFO("SrbDecoder.cpp: Number of Words/Edges is Null !");

	for (uint iEdge = 0; iEdge < nEdges; iEdge++) {

		SrbData* srbData = (SrbData*) payload + 2 + iEdge + 2;

		//first edge (16 LSB in dataword)
		edgeTimes[iEdge] = ((double) srbData->fineTime) * ClockPeriod / 32.; //fine time - later converted to global
		edgeStrawIDs[iEdge] = (uint) srbData->strawID;
		edgeSrbIDs[iEdge] = SrbID;
		edgeErrorFlags[iEdge] = (uint) srbData->errorFlag;
		edgeIsLeading[iEdge] = !((bool) srbData->edgeType);

	}

	int NSlots = 16;
	int NEdgesInSlot = 0;
	int64_t slotTime = 0;
	nEdges_tot = 0;
	for (uint iSlot = 0; iSlot < NSlots; iSlot++) {
		SrbTimeSlot* srbTimeSlot = (SrbTimeSlot*) payload + (2 + nWords) * 4 + iSlot;
		NEdgesInSlot = (uint) srbTimeSlot->SlotCounter;
		for (int iEdgeInSlot = 0; iEdgeInSlot < NEdgesInSlot; iEdgeInSlot++) {
			slotTime = firstTSCoarseTime + iSlot;
			edgeTimes[nEdges_tot + iEdgeInSlot] += slotTime * ClockPeriod;
			//LOG_INFO(std::hex << timestamp << std::dec << " " << std::hex << firstTSCoarseTime+timestamp << std::dec << " " << std::hex << SrbID << std::dec << " " << std::setprecision(6) << edgeTimes[nEdges_tot + iEdgeInSlot] << " " << edgeIsLeading[nEdges_tot + iEdgeInSlot] << " "  << std::hex << edgeStrawIDs[nEdges_tot + iEdgeInSlot]  << std::dec);
		}
		nEdges_tot += NEdgesInSlot;
	}
}

//	LOG_INFO("SrbDecoder.cpp: Analyzed SRB ID " << fragmentNumber_ << " - Number of edges found " << nEdges_tot);

}
