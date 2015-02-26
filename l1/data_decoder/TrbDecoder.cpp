/*
 * TrbDecoder.cpp
 *
 *  Created on: Nov 24, 2014
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

#include "TrbDecoder.h"
#include "../cedar_algorithm/tdcb_buffer.h"
#include <options/Logging.h>
#include <eventBuilding/Event.h>
#include <l0/MEPFragment.h>
#include <stdlib.h>
#include <cstdint>

namespace na62 {

TrbDecoder::TrbDecoder() {
	nhits = 0;
	nhits_tot = 0;
	nWords = 0;
	boardHeader = 0;
	fpgaHeader = new FPGADataHeader*[maxNFPGAs];
	frameHeader = new FrameDataHeader**[maxNFPGAs];
	tdc_data = new TrbData*[maxNhits];
	for (int i = 0; i < maxNFPGAs; i++) {
		frameHeader[i] = new FrameDataHeader*[maxNFrames];
	}

//	noHitsPerTrb = new uint[maxNTEL62s];
	noFrame = new uint[maxNFPGAs];
	noNonEmptyFrame = new uint[maxNFPGAs];
	FPGAID = new uint[maxNFPGAs];
	errFlags = new uint[maxNFPGAs];
	coarseFrameTime = new uint16_t*[maxNFPGAs];
	nWordsPerFrame = new uint*[maxNFPGAs];
	for (int i = 0; i < maxNFPGAs; i++) {
		coarseFrameTime[i] = new uint16_t[maxNFrames];
		nWordsPerFrame[i] = new uint[maxNFrames];
	}

	time = new uint32_t[maxNhits];
	chID = new uint[maxNhits];
	tdcID = new uint[maxNhits];
	ID = new uint[maxNhits];
	trbID = new uint[maxNhits];
}

TrbDecoder::~TrbDecoder() {
	// TODO Auto-generated destructor stub
	delete fpgaHeader;
	delete frameHeader;
	delete tdc_data;
//	delete noHitsPerTrb;
	delete noFrame;
	delete noNonEmptyFrame;
	delete FPGAID;
	delete errFlags;
	delete coarseFrameTime;
	delete nWordsPerFrame;
	delete time;
	delete chID;
	delete tdcID;
	delete ID;
	delete trbID;
}

void TrbDecoder::SetHits(uint trbNum, l0::MEPFragment* trbDataFragment) {
	char * payload = trbDataFragment->getPayload();
	boardHeader = (TrbDataHeader*) payload;

//	LOG_INFO << "maxNFPGA " << maxNFPGA << ENDL;
//	LOG_INFO << "maxNFrame " << maxNFrame << ENDL;
//	LOG_INFO << "Flags " << ((int) cedarHeader->flags) << ENDL;
//	LOG_INFO << "L0 trigger type " << ((int) cedarHeader->triggerType) << ENDL;
//	LOG_INFO << "Source (Tel62) sub-ID " << ((int) cedarHeader->sourceSubID) << ENDL;
//	LOG_INFO << "Format " << ((int) cedarHeader->format) << ENDL;

	for (int iFPGA = 0; iFPGA < maxNFPGAs; iFPGA++) {
//		printf ("writing getpayload() + %d\n",1+iFPGA+nWords);
		fpgaHeader[iFPGA] = (FPGADataHeader*) payload + 1 + iFPGA + nWords;

		noFrame[iFPGA] = (uint) fpgaHeader[iFPGA]->noFrame;
		noNonEmptyFrame[iFPGA] = (uint) fpgaHeader[iFPGA]->noNonEmptyFrame;
		FPGAID[iFPGA] = (uint) fpgaHeader[iFPGA]->FPGAID;
		errFlags[iFPGA] = (uint) fpgaHeader[iFPGA]->errFlags;

		//LOG_INFO<< "noFrame[" << iFPGA << "] " << noFrame[iFPGA] << ENDL;
		//LOG_INFO<< "noNonEmptyFrame[" << iFPGA << "] " << noNonEmptyFrame[iFPGA] << ENDL;
		//LOG_INFO<< "FPGAID[" << iFPGA << "] " << FPGAID[iFPGA] << ENDL;
		//LOG_INFO<< "errFlags[" << iFPGA << "] " << errFlags[iFPGA] << ENDL;

		for (int iFrame = 0; iFrame < maxNFrames; iFrame++) {
//			printf ("writing getpayload() + %d\n",2+iFPGA+nWords);
			frameHeader[iFPGA][iFrame] = (FrameDataHeader*) payload + 2 + iFPGA
					+ nWords;

			coarseFrameTime[iFPGA][iFrame] =
					(uint16_t) frameHeader[iFPGA][iFrame]->coarseFrameTime;
			nWordsPerFrame[iFPGA][iFrame] =
					(uint) frameHeader[iFPGA][iFrame]->nWordsPerFrame;
			nWords += (uint) frameHeader[iFPGA][iFrame]->nWordsPerFrame;

			//LOG_INFO<< "FrameTimestamp[" << iFPGA << "][" << iFrame << "] " << std::hex << coarseFrameTime[iFPGA][iFrame] << std::dec << ENDL;
			//LOG_INFO<< "nWordsPerFrame[" << iFPGA << "][" << iFrame << "] " << nWordsPerFrame[iFPGA][iFrame] << ENDL;
			//LOG_INFO<< "nWords " << nWords << ENDL;

			if (nWordsPerFrame[iFPGA][iFrame])
				nhits = nWordsPerFrame[iFPGA][iFrame] - 1;
			else
				LOG_INFO<< "TrbDecoder.cpp: Number of Words in Frame is Null !" << ENDL;

				//LOG_INFO<< "nhits " << nhits << ENDL;
			if (nhits) {
				for (uint ihit = 0; ihit < nhits; ihit++) {
//					printf("writing getpayload() + %d\n",2 + iFPGA + nWords - nhits + ihit);
					tdc_data[ihit + nhits_tot] = (TrbData*) payload + 2 + iFPGA
							+ nWords - nhits + ihit;

//					printf("tdc word %08x\n",(uint32_t) tdc_data[ihit + nhits_tot]->tdcWord);

					time[ihit + nhits_tot] = (uint32_t) tdc_data[ihit
							+ nhits_tot]->Time;
					chID[ihit + nhits_tot] =
							(uint) tdc_data[ihit + nhits_tot]->chID;
					tdcID[ihit + nhits_tot] =
							(uint) tdc_data[ihit + nhits_tot]->tdcID;
					ID[ihit + nhits_tot] =
							(uint) tdc_data[ihit + nhits_tot]->ID;
					trbID[ihit + nhits_tot] = trbNum;

//					LOG_INFO<< "time[" << ihit + nhits_tot << "] " << std::hex << time[ihit + nhits_tot] << std::dec << ENDL;
//					LOG_INFO<< "chID[" << ihit + nhits_tot << "] " << chID[ihit + nhits_tot] << ENDL;
//					LOG_INFO<< "tdcID["<< ihit + nhits_tot << "] " << tdcID[ihit + nhits_tot] << ENDL;
//					LOG_INFO<< "ID[" << ihit + nhits_tot << "] " << ID[ihit + nhits_tot] << ENDL;
//					LOG_INFO<< "trbID[" << ihit + nhits_tot << "] " << trbID[ihit + nhits_tot] << ENDL;

					if (ihit == (nhits - 1)) {
						nhits_tot += nhits;
					}
				}
			}
		}
	}
//	LOG_INFO<<"Analysing Tel62 " << trbNum << " Number of hits found " << nhits_tot << ENDL;
}

}
