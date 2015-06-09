/*
 * TrbDecoder.cpp
 *
 *  Created on: Nov 24, 2014
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

#include "TrbFragmentDecoder.h"

#include <stdlib.h>
#include <cstdint>
#include <options/Logging.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>

namespace na62 {

TrbFragmentDecoder::TrbFragmentDecoder() :
		edgeTimes(nullptr), edgeChIDs(nullptr), edgeTdcIDs(nullptr), edgeIsLeading(
				nullptr), subevent_(nullptr), fragmentNumber_(UINT_FAST16_MAX) {
	frameTS = 0;
	time = 0;
	nEdges_tot = 0;
}

TrbFragmentDecoder::~TrbFragmentDecoder() {
	if (isReady()) {
		delete[] edgeTimes;
		delete[] edgeChIDs;
		delete[] edgeTdcIDs;
		delete[] edgeIsLeading;
	}
}

/**
 * TODO: move getData documentation in TrbDecoder.h and update it
 * @params uint trbNum This is an index running on the Tel62 boards used by the sub-detector
 *
 * @params l0::MEPFragment* trbDataFragment This is a pointer to the data fragment received by the selected Tel62 board
 * TODO: try and create a unit test for Decoder!!!
 * TODO: have you thought about corrupted data?
 */
void TrbFragmentDecoder::readData(uint_fast32_t timestamp) {
	// Don't run again if we already read the data
	if (edgeTimes != nullptr) {
		return;
	}

	const l0::MEPFragment* const trbDataFragment = subevent_->getFragment(
			fragmentNumber_);
	/*
	 * Each word is 4 bytes: there is 1 board header and at least 1 FPGA header and 1 Frame header
	 * -> use this to estimate the maximum number of words
	 */
	const uint maxNwords = (trbDataFragment->getPayloadLength() / 4) - 2;
	edgeTimes = new uint64_t[maxNwords];
	edgeChIDs = new uint_fast8_t[maxNwords];
	edgeTdcIDs = new uint_fast8_t[maxNwords];
	edgeIsLeading = new bool[maxNwords];

	const char* const payload = trbDataFragment->getPayload();

	const TrbDataHeader* const boardHeader =
			reinterpret_cast<const TrbDataHeader*>(payload);

	//LOG_INFO<< "FPGA Flags " << (uint) boardHeader->fpgaFlags << ENDL;
	//LOG_INFO<< "L0 trigger type " << (uint) boardHeader->triggerType << ENDL;
	//LOG_INFO<< "Source (Tel62) sub-ID " << (uint) boardHeader->sourceSubID << ENDL;
	//LOG_INFO<< "Format " << (uint) boardHeader->format << ENDL;

	const uint nFPGAs = boardHeader->getNumberOfFPGAs();

	//LOG_INFO<< "Number of FPGAs (from boardHeader) " << nFPGAs << ENDL;

	uint_fast16_t nWords_tot = 0;
	for (uint iFPGA = 0; iFPGA != nFPGAs; iFPGA++) {
		//printf("writing getpayload() + %d\n", 1 + iFPGA + nWords_tot);
		FPGADataHeader* fpgaHeader = (FPGADataHeader*) payload + 1 + iFPGA
				+ nWords_tot;

		//LOG_INFO<< "Number of (25ns long) Frames " << (uint) fpgaHeader->noFrames << ENDL;
		//LOG_INFO<< "Number of non-empty Frames " << (uint) fpgaHeader->noNonEmptyFrames<< ENDL;
		//LOG_INFO<< "FPGA ID " << (uint) fpgaHeader->FPGAID<< ENDL;
		//LOG_INFO<< "Error Flags " << (uint) fpgaHeader->errFlags<< ENDL;

		////////// 2015 DATA FORMAT with suppression of empty frames ////////////////////
		const uint_fast8_t nFrames = fpgaHeader->noNonEmptyFrames;
		//LOG_INFO<< "Number of (Non Empty) Frames (from fpgaHeader) " << nFrames << ENDL;

		for (uint iFrame = 0; iFrame != nFrames; iFrame++) {
			//printf("writing getpayload() + %d\n", 2 + iFPGA + nWords_tot);
			FrameDataHeader* frameHeader = (FrameDataHeader*) payload + 2
					+ iFPGA + nWords_tot;

			//LOG_INFO<< "Number of Words in Frame " << (uint) frameHeader->nWords<< ENDL;
			//LOG_INFO<< "Frame Timestamp " << (uint) frameHeader->frameTimeStamp<< ENDL;

			const uint_fast16_t nWordsOfCurrentFrame =
					(uint) frameHeader->nWords;
			nWords_tot += nWordsOfCurrentFrame;
			//LOG_INFO<< "Number of Words  " << nWords_tot << ENDL;

			frameTS = (frameHeader->frameTimeStamp & 0x0000ffff)
					+ (timestamp & 0xffff0000);
//			LOG_INFO<< "Event Timestamp " << std::hex << timestamp << std::dec << ENDL;
//			LOG_INFO<< "FrameTS " << std::hex << frameTS << std::dec << ENDL;

			if ((timestamp & 0xf000) == 0xf000 && (frameTS & 0xf000) == 0x0000)
				frameTS += 0x10000; //16 bits overflow
			if ((timestamp & 0xf000) == 0x0000 && (frameTS & 0xf000) == 0xf000)
				frameTS -= 0x10000; //16 bits overflow

			const uint nEdges = nWordsOfCurrentFrame - 1;
			if (!nWordsOfCurrentFrame)
				LOG_ERROR<< "TrbDecoder.cpp: Number of Words in Frame is Null !" << ENDL;

//				LOG_INFO<< "nEdges " << nEdges << ENDL;
			if (nEdges) {
				for (uint iEdge = 0; iEdge < nEdges; iEdge++) {
					//printf("writing getpayload() + %d\n",2 + iFPGA + nWords_tot - nEdges + iEdge);
					TrbData* tdcData = (TrbData*) payload + 2 + iFPGA
							+ nWords_tot - nEdges + iEdge;

					/*
					 * TODO: let the user decide what data he needs and remove the unwanted parts in compile time
					 */
//					edge_times[iEdge + nEdges_tot] = ((time - timestamp * 256.) * 0.097464731802);
					/*
					 * TODO: edge_times is still a uint64_t*: is this necessary?
					 *
					 */
					edgeTimes[iEdge + nEdges_tot] = (tdcData->time & 0x0007ffff)
							+ ((frameTS & 0xfffff800) * 0x100);

					edgeChIDs[iEdge + nEdges_tot] = (uint) tdcData->chID;
					edgeTdcIDs[iEdge + nEdges_tot] = (uint) tdcData->tdcID;
					edgeIsLeading[iEdge + nEdges_tot] = tdcData->ID == 0x4;

//					LOG_INFO<< "edgeChIDs[" << iEdge + nEdges_tot << "] " << (uint) edgeChIDs[iEdge + nEdges_tot] << ENDL;
//					LOG_INFO<< "edgeTdcIDs[" << iEdge + nEdges_tot << "] " << (uint) edgeTdcIDs[iEdge + nEdges_tot] << ENDL;
//					LOG_INFO<< "edgeIsLeading["<< iEdge + nEdges_tot << "] " << edgeIsLeading[iEdge + nEdges_tot] << ENDL;
//					LOG_INFO<< "edgeTimes[" << iEdge + nEdges_tot << "] " << std::hex << edgeTimes[iEdge + nEdges_tot] << std::dec << ENDL;

//					LOG_INFO<< "TIME (ns) " << ((edgeTimes[iEdge+nEdges_tot] - timestamp* 256.) * 0.097464731802) << ENDL;

					if (iEdge == (nEdges - 1)) {
						nEdges_tot += nEdges;
					}
				}
			}
		}
	}
//	LOG_INFO<<"TrbDecoder.cpp: Analysed Tel62 ID " << fragmentNumber_ << " - Number of edges found " << nEdges_tot << ENDL;
}

}
