/*
 * TrbDecoder.cpp
 *
 *  Created on: Nov 24, 2014
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

#include "TrbDecoder.h"
#include <options/Logging.h>
#include <eventBuilding/Event.h>
#include <l0/MEPFragment.h>
#include <stdlib.h>
#include <cstdint>

namespace na62 {

TrbDecoder::TrbDecoder() {
	frameTS = 0;
	time = 0;
	nFPGAs = 0;
	nFrames = 0;
	nWordsPerFrame = 0;
	nWords_tot = 0;
	nEdges = 0;
	nEdges_tot = 0;
	boardHeader = 0;
	fpgaHeader = 0;
	frameHeader = 0;
	tdcData = 0;

	edge_times = new double[maxNhits];
	edge_chIDs = new uint[maxNhits];
	edge_tdcIDs = new uint[maxNhits];
	edge_IDs = new uint[maxNhits];
	edge_trbIDs = new uint[maxNhits];
}

TrbDecoder::~TrbDecoder() {
	// TODO Auto-generated destructor stub
	delete edge_times;
	delete edge_chIDs;
	delete edge_tdcIDs;
	delete edge_IDs;
	delete edge_trbIDs;
}

int TrbDecoder::SetNFPGAs(uint fpgaFlags) {
	int nFPGAs = 0;

	if (fpgaFlags == 15)
		nFPGAs = 4;
	else if (fpgaFlags == 7)
		nFPGAs = 3;
	else if (fpgaFlags == 3)
		nFPGAs = 2;
	else if (fpgaFlags == 1)
		nFPGAs = 1;
	else
		LOG_ERROR<<"TrbDecoder.cpp: Number of FPGAs not identified !" << ENDL;

	return nFPGAs;
}

/**
 *
 * @params uint trbNum This is an index running on the Tel62 boards used by the sub-detector
 *
 * @params l0::MEPFragment* trbDataFragment This is a pointer to the data fragment received by the selected Tel62 board
 *
 */

void TrbDecoder::GetData(uint trbNum, l0::MEPFragment* trbDataFragment,
		Event* event) {
	char * payload = trbDataFragment->getPayload();
	boardHeader = (TrbDataHeader*) payload;

	//LOG_INFO<< "FPGA Flags " << (uint) boardHeader->fpgaFlags << ENDL;
	//LOG_INFO<< "L0 trigger type " << (uint) boardHeader->triggerType << ENDL;
	//LOG_INFO<< "Source (Tel62) sub-ID " << (uint) boardHeader->sourceSubID << ENDL;
	//LOG_INFO<< "Format " << (uint) boardHeader->format << ENDL;

	nFPGAs = SetNFPGAs((uint) boardHeader->fpgaFlags);
	//LOG_INFO<< "Number of FPGAs (from boardHeader) " << nFPGAs << ENDL;

	for (int iFPGA = 0; iFPGA < nFPGAs; iFPGA++) {
		//printf("writing getpayload() + %d\n", 1 + iFPGA + nWords_tot);
		fpgaHeader = (FPGADataHeader*) payload + 1 + iFPGA + nWords_tot;

		//LOG_INFO<< "Number of (25ns long) Frames " << (uint) fpgaHeader->noFrames << ENDL;
		//LOG_INFO<< "Number of non-empty Frames " << (uint) fpgaHeader->noNonEmptyFrames<< ENDL;
		//LOG_INFO<< "FPGA ID " << (uint) fpgaHeader->FPGAID<< ENDL;
		//LOG_INFO<< "Error Flags " << (uint) fpgaHeader->errFlags<< ENDL;

		nFrames = (uint) fpgaHeader->noFrames;
		//LOG_INFO<< "Number of Frames (from fpgaHeader) " << nFrames << ENDL;

		for (int iFrame = 0; iFrame < nFrames; iFrame++) {
			//printf("writing getpayload() + %d\n", 2 + iFPGA + nWords_tot);
			frameHeader = (FrameDataHeader*) payload + 2 + iFPGA + nWords_tot;

			//LOG_INFO<< "Number of Words in Frame " << (uint) frameHeader->nWords<< ENDL;
			//LOG_INFO<< "Frame Timestamp " << (uint) frameHeader->frameTimeStamp<< ENDL;

			nWordsPerFrame = (uint) frameHeader->nWords;
			nWords_tot += nWordsPerFrame;
			//LOG_INFO<< "Number of Words  " << nWords_tot << ENDL;

			frameTS = (frameHeader->frameTimeStamp & 0x0000ffff) + (event->getTimestamp() & 0xffff0000);
			//LOG_INFO<< "Event Timestamp " << std::hex << event->getTimestamp() << std::dec << ENDL;
			//LOG_INFO<< "FrameTS " << std::hex << frameTS << std::dec << ENDL;

			if ((event->getTimestamp() & 0xf000) == 0xf000
					&& (frameTS & 0xf000) == 0x0000)
				frameTS += 0x10000; //16 bits overflow
			if ((event->getTimestamp() & 0xf000) == 0x0000
					&& (frameTS & 0xf000) == 0xf000)
				frameTS -= 0x10000; //16 bits overflow

			if (nWordsPerFrame)
				nEdges = nWordsPerFrame - 1;
			else
				LOG_ERROR<< "TrbDecoder.cpp: Number of Words in Frame is Null !" << ENDL;
				//LOG_INFO<< "nEdges " << nEdges << ENDL;
			if (nEdges) {
				for (uint iEdge = 0; iEdge < nEdges; iEdge++) {
					//printf("writing getpayload() + %d\n",2 + iFPGA + nWords_tot - nEdges + iEdge);
					tdcData = (TrbData*) payload + 2 + iFPGA + nWords_tot
							- nEdges + iEdge;

					time = (tdcData->time & 0x0007ffff) + ((frameTS & 0xfffff800)*0x100);
					//LOG_INFO<< "Time " << std::hex << time << std::dec << ENDL;

					edge_times[iEdge + nEdges_tot] = ((time
							- event->getTimestamp() * 256.) * 0.097464731802);
					edge_chIDs[iEdge + nEdges_tot] = (uint) tdcData->chID;
					edge_tdcIDs[iEdge + nEdges_tot] = (uint) tdcData->tdcID;
					edge_IDs[iEdge + nEdges_tot] = (uint) tdcData->ID;
					edge_trbIDs[iEdge + nEdges_tot] = trbNum;

					//LOG_INFO<< "edge_IDs[" << iEdge + nEdges_tot << "] " << edge_IDs[iEdge + nEdges_tot] << ENDL;
					//LOG_INFO<< "edge_chIDs[" << iEdge + nEdges_tot << "] " << edge_chIDs[iEdge + nEdges_tot] << ENDL;
					//LOG_INFO<< "edge_tdcIDs["<< iEdge + nEdges_tot << "] " << edge_tdcIDs[iEdge + nEdges_tot] << ENDL;
					//LOG_INFO<< "edge_times[" << iEdge + nEdges_tot << "] " << edge_times[iEdge + nEdges_tot] << ENDL;
					//LOG_INFO<< "edge_trbIDs[" << iEdge + nEdges_tot << "] " << edge_trbIDs[iEdge + nEdges_tot] << ENDL;

					if (iEdge == (nEdges - 1)) {
						nEdges_tot += nEdges;
					}
				}
			}
		}
	}
	//LOG_INFO<<"TrbDecoder.cpp: Analysed Tel62 ID " << trbNum << " - Number of edges found " << nEdges_tot << ENDL;
}

}
