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

#ifndef ONLINEHLT
#include <l0/Subevent.h>
#else
#include <l0/offline/Subevent.h>
#endif

namespace na62 {

TrbFragmentDecoder::TrbFragmentDecoder() :
		edgeTimes(nullptr), edgeChIDs(nullptr), edgeTdcIDs(nullptr), edgeIsLeading(
				nullptr), subevent_(nullptr), fragmentNumber_(UINT_FAST16_MAX) {
	frameTS = 0;
	time = 0;
	nEdges_tot = 0;
	isBadFrag_ = false;
	sourceSubId_ = UINT_FAST16_MAX;
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
    //std::cout<<"Executing readData(): " << timestamp <<std::endl;
	// Don't run again if we already read the data
	if (edgeTimes != nullptr) {
		return;
	}

	const l0::MEPFragment* const trbDataFragment = subevent_->getFragment(
			fragmentNumber_);

//	LOG_INFO("SourceID " << (uint) trbDataFragment->getSourceID() << " Detector Name " << SourceIDManager::sourceIdToDetectorName((uint) trbDataFragment->getSourceID()));
//	LOG_INFO("SourceSubID " << (uint) trbDataFragment->getSourceSubID());
//	LOG_INFO("trbData " << trbDataFragment->getPayloadLength());
	if (!trbDataFragment->getPayloadLength()) {

#ifndef ONLINEHLT
		LOG_ERROR("DetID " << SourceIDManager::sourceIdToDetectorName((uint) trbDataFragment->getSourceID()) << " SubID " << (uint) trbDataFragment->getSourceSubID() << " : The packet payload is empty");
#else
		LOG_ERROR("DetID " << (uint) trbDataFragment->getSourceID() << " SubID " << (uint) trbDataFragment->getSourceSubID() << " : The packet payload is empty");
#endif
		isBadFrag_ = true;
		return;
	}

	const char* const payload = trbDataFragment->getPayload();

	const TrbDataHeader* const boardHeader =
			reinterpret_cast<const TrbDataHeader*>(payload);

//	LOG_INFO("FPGA Flags " << (uint) boardHeader->fpgaFlags);
//	LOG_INFO("L0 trigger type " << (uint) boardHeader->triggerType);
//	LOG_INFO("Source (Tel62) sub-ID " << (uint) boardHeader->sourceSubID);
//	LOG_INFO("Format " << (uint) boardHeader->format);

	/*
	 * Each word is 4 bytes: there is 1 board header and at least 1 FPGA header
	 * -> use this to estimate the maximum number of words
	 */

	const uint maxNwords = (trbDataFragment->getPayloadLength() / 4);

	if (maxNwords <= 0) {
#ifndef ONLINEHLT
		LOG_ERROR(
				"DetID " << SourceIDManager::sourceIdToDetectorName((uint) trbDataFragment->getSourceID()) <<
				" SubID " << (uint) trbDataFragment->getSourceSubID() <<
				" (Tel62) Trigger type " << (uint) boardHeader->triggerType <<
				" FPGA Flags " << (uint) boardHeader->fpgaFlags <<
				" : The packet payload is not as expected !!!");
#else
		LOG_ERROR("DetID " << (uint) trbDataFragment->getSourceID() << " SubID " << (uint) trbDataFragment->getSourceSubID() << " : The packet payload is not as expected !!!");
#endif
		//throw NA62Error("The packet payload is not as expected !!!");
		isBadFrag_ = true;
		return;
	}

	if ((uint) boardHeader->triggerType & 0x80) {
#ifndef ONLINEHLT
		LOG_ERROR(
			  "DetID " << SourceIDManager::sourceIdToDetectorName((uint) trbDataFragment->getSourceID()) <<
			  " SubID " << (uint) trbDataFragment->getSourceSubID() <<
			  " (Tel62) Trigger type " << (uint) boardHeader->triggerType <<
			  " FPGA Flags " << (uint) boardHeader->fpgaFlags <<
			  " : Special trigger detected in L1 TrbFragmentDecoder! Found data packet payload (in 32bit words) = " << maxNwords <<
			  " Return as different from physics packet");
#else
		LOG_ERROR(
			  " SubID " << (uint) trbDataFragment->getSourceSubID() <<
			  " (Tel62) Trigger type " << (uint) boardHeader->triggerType <<
			  " FPGA Flags " << (uint) boardHeader->fpgaFlags <<
			  " : Special trigger detected in L1 TrbFragmentDecoder! Found data packet payload (in 32bit words) = " << maxNwords <<
			  " Return as different from physics packet");
#endif			      
		return;
	}

	sourceSubId_ = (uint_fast16_t) boardHeader->sourceSubID;

	const uint nFPGAs = boardHeader->getNumberOfFPGAs();
//	LOG_INFO("Number of FPGAs (from boardHeader) " << nFPGAs);

	if (!((uint)boardHeader->triggerType & 0x80) && (!boardHeader->fpgaFlags || nFPGAs > 4)) {
#ifndef ONLINEHLT
		LOG_ERROR(
				"DetID " << SourceIDManager::sourceIdToDetectorName((uint) trbDataFragment->getSourceID()) <<
				" SubID " << (uint) trbDataFragment->getSourceSubID() <<
				" (Tel62) Trigger type " << (uint) boardHeader->triggerType <<
				" FPGA Flags " << (uint) boardHeader->fpgaFlags <<
				" number of FPGAs " << (uint)nFPGAs <<
				" : Physics trigger - FPGA Flags or Number of FPGAs is not as expected !");
#else
		LOG_ERROR("DetID " << (uint) trbDataFragment->getSourceID() << " SubID " << (uint) trbDataFragment->getSourceSubID() << " : FPGA Flags or Number of FPGAs is not as expected !");
#endif
		isBadFrag_ = true;
		return;
	}

	edgeTimes = new uint64_t[maxNwords];
	edgeChIDs = new uint_fast8_t[maxNwords];
	edgeTdcIDs = new uint_fast8_t[maxNwords];
	edgeIsLeading = new bool[maxNwords];

	//initialize
	for (uint i = 0; i < maxNwords; i++) {
		edgeTimes[i] = 0;
		edgeChIDs[i] = 0;
		edgeTdcIDs[i] = 0;
		edgeIsLeading[i] = 0;
	}
	uint_fast16_t nWords_tot = 0;

	for (uint iFPGA = 0; iFPGA != nFPGAs; iFPGA++) {
//		printf("writing getpayload() + %d\n", 1 + iFPGA + nWords_tot);
		FPGADataHeader* fpgaHeader = (FPGADataHeader*) payload + 1 + iFPGA
				+ nWords_tot;

//		LOG_INFO("Number of (25ns long) Frames " << (uint) fpgaHeader->noFrames);
//		LOG_INFO("Number of non-empty Frames " << (uint) fpgaHeader->noNonEmptyFrames);
//		LOG_INFO("FPGA ID " << (uint) fpgaHeader->FPGAID);
//		LOG_INFO("Error Flags " << (uint) fpgaHeader->errFlags);

		/*
		 * In 2015 DATA FORMAT empty frames are suppressed
		 */
		const uint_fast8_t nFrames = fpgaHeader->noNonEmptyFrames;

		for (uint iFrame = 0; iFrame != nFrames; iFrame++) {
//			printf("writing getpayload() + %d\n", 2 + iFPGA + nWords_tot);
			FrameDataHeader* frameHeader = (FrameDataHeader*) payload + 2
					+ iFPGA + nWords_tot;

			//LOG_INFO("Number of Words in Frame " << (uint) frameHeader->nWords);
			//LOG_INFO("Frame Timestamp " << (uint) frameHeader->frameTimeStamp);

			const uint_fast16_t nWordsOfCurrentFrame =
					(uint) frameHeader->nWords;
			if (!nWordsOfCurrentFrame) {

#ifndef ONLINEHLT
				LOG_ERROR("DetID " << SourceIDManager::sourceIdToDetectorName((uint) trbDataFragment->getSourceID()) << " SubID " << (uint) trbDataFragment->getSourceSubID() << " : Number of Words in Frame is Null !" << std::hex << (int) (trbDataFragment->getSourceID()) << ":"
				<< (int)(trbDataFragment->getSourceSubID()));
#else
				LOG_ERROR("DetID " << (uint) trbDataFragment->getSourceID() << " SubID " << (uint) trbDataFragment->getSourceSubID() << " : Number of Words in Frame is Null !" << std::hex << (int) (trbDataFragment->getSourceID()) << ":"
				<< (int)(trbDataFragment->getSourceSubID()));
#endif
				isBadFrag_ = true;
				return;
			}
			nWords_tot += nWordsOfCurrentFrame;
			//LOG_INFO("Number of Words  " << nWords_tot);

			frameTS = (frameHeader->frameTimeStamp & 0x0000ffff)
					+ (timestamp & 0xffff0000);
			//LOG_INFO("Event Timestamp " << std::hex << timestamp << std::dec);
			//LOG_INFO("FrameTS " << std::hex << frameTS << std::dec);

			if ((timestamp & 0xf000) == 0xf000 && (frameTS & 0xf000) == 0x0000)
				frameTS += 0x10000; //16 bits overflow
			if ((timestamp & 0xf000) == 0x0000 && (frameTS & 0xf000) == 0xf000)
				frameTS -= 0x10000; //16 bits overflow

			const uint nEdges = nWordsOfCurrentFrame - 1;
			//LOG_INFO("nEdges " << nEdges << " Max Number of words: " << std::dec << nWords_tot);

			for (uint iEdge = 0; iEdge < nEdges; iEdge++) {
				if (2 + iFPGA + nWords_tot - nEdges + iEdge > trbDataFragment->getPayloadLength() / 4) {
					LOG_ERROR("!!!! Trying to access to a word outside the MEP Fragment");
					isBadFrag_ = true;
					return;
				}
				//LOG_INFO("Payload legnth: " << trbDataFragment->getPayloadLength() / 4 << " index: " << payload + 2 + iFPGA + nWords_tot);
				//printf("writing getpayload() + %d\n", 2 + iFPGA + nWords_tot - nEdges + iEdge);
				TrbData* tdcData = (TrbData*) payload + 2 + iFPGA + nWords_tot
						- (nEdges - iEdge);
                //int delta = 2 + iFPGA + nWords_tot - (2 + iFPGA + nWords_tot - (nEdges - iEdge));


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
				edgeTdcIDs[iEdge + nEdges_tot] = (uint) tdcData->tdcID + fpgaHeader->FPGAID*4;
				edgeIsLeading[iEdge + nEdges_tot] = tdcData->ID == 0x4;

				//LOG_INFO("edgeChIDs[" << iEdge + nEdges_tot << "] " << (uint) edgeChIDs[iEdge + nEdges_tot]);
				//LOG_INFO("edgeTdcIDs[" << iEdge + nEdges_tot << "] " << (uint) edgeTdcIDs[iEdge + nEdges_tot]);
				//LOG_INFO("edgeIsLeading["<< iEdge + nEdges_tot << "] " << edgeIsLeading[iEdge + nEdges_tot]);
				//LOG_INFO("edgeTimes[" << iEdge + nEdges_tot << "] " << std::hex << edgeTimes[iEdge + nEdges_tot] << std::dec);

				//LOG_INFO(iEdge << " TIME (ns) " << ((edgeTimes[iEdge+nEdges_tot] - timestamp* 256.) * 0.097464731802));

			}

			nEdges_tot += nEdges;
		}
		if (fpgaHeader->errFlags & 0x1) {
//			printf("writing getpayload() + %d\n", 2 + iFPGA + nWords_tot);
			ErrorDataHeader* errHeader = (ErrorDataHeader*) payload + 2 + iFPGA
					+ nWords_tot;

			if ((((uint) errHeader->frame1ErrWords) != 0xff)
					&& ((nWords_tot + nFPGAs + 1)
							!= (trbDataFragment->getPayloadLength() / 4))) {


				/*
				 * In 2015 DATA FORMAT error words can be present at the end of each FPGA block
				 */
				const uint_fast16_t nErrWords = errHeader->nErrWords;
				if (!nErrWords) {

#ifndef ONLINEHLT
					LOG_ERROR("DetID " << SourceIDManager::sourceIdToDetectorName((uint) trbDataFragment->getSourceID()) << " SubID " << (uint) trbDataFragment->getSourceSubID() << " : Number of ErrWords is Null but Err Flag says otherwise!" << std::hex << (int) (trbDataFragment->getSourceID()) << ":" << (int)(trbDataFragment->getSourceSubID()));
#else
					LOG_ERROR("DetID " << (uint) trbDataFragment->getSourceID() << " SubID " << (uint) trbDataFragment->getSourceSubID() << " : Number of ErrWords is Null but Err Flag says otherwise!" << std::hex << (int) (trbDataFragment->getSourceID()) << ":" << (int)(trbDataFragment->getSourceSubID()));
#endif
					isBadFrag_ = true;
					return;
				}
				//const uint nErrors = nErrWords - 1;
				//for (uint iErr = 0; iErr != nErrors; iErr++) {
				//	//printf("writing getpayload() + %d\n", 3 + iFPGA + nWords_tot + iErr);
				//	ErrData* errData = (ErrData*) payload + 3 + iFPGA + nWords_tot + iErr;
				//	//LOG_INFO("Error Word " << std::hex << (uint) errData->errWord << std::dec);
				//}
				nWords_tot += nErrWords;
				//LOG_INFO("Number of Words  " << nWords_tot);
	 	} else {
				//LOG_INFO("Err flag " << (uint)fpgaHeader->errFlags);
				fpgaHeader->errFlags = (fpgaHeader->errFlags & 0xfe);
				//LOG_INFO("Err flag " << (uint)fpgaHeader->errFlags);
				//LOG_INFO(" **********Bit flip");

			}
		}
	}
}

}
