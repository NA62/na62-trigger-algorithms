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
	fpgaHeader = new FPGADataHeader*[maxNFPGA];
	frameHeader = new FrameDataHeader**[maxNFPGA];
	tdc_data = new TrbData*[maxNhit];
	for (int i = 0; i < maxNFPGA; i++) {
		frameHeader[i] = new FrameDataHeader*[maxNFrame];
	}

	noFrame = new uint[maxNFPGA];
	noNonEmptyFrame = new uint[maxNFPGA];
	FPGAID = new uint[maxNFPGA];
	errFlags = new uint[maxNFPGA];
	coarseFrameTime = new uint16_t*[maxNFPGA];
	nWordsPerFrame = new uint*[maxNFPGA];
	for (int i = 0; i < maxNFPGA; i++) {
		coarseFrameTime[i] = new uint16_t[maxNFrame];
		nWordsPerFrame[i] = new uint[maxNFrame];
	}

	time = new uint32_t[maxNhit];
	chID = new uint[maxNhit];
	tdcID = new uint[maxNhit];
	ID = new uint[maxNhit];

}

TrbDecoder::~TrbDecoder() {
	// TODO Auto-generated destructor stub
	delete fpgaHeader;
	delete frameHeader;
	delete tdc_data;
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
}

void TrbDecoder::SetHits(l0::MEPFragment* trbDataFragment) {
	char * payload = trbDataFragment->getPayload();
	boardHeader = (TrbDataHeader*) payload;

//	LOG_INFO << "maxNFPGA " << maxNFPGA << ENDL;
//	LOG_INFO << "maxNFrame " << maxNFrame << ENDL;
//
//	LOG_INFO << "Flags " << ((int) cedarHeader->flags) << ENDL;
//	LOG_INFO << "L0 trigger type " << ((int) cedarHeader->triggerType) << ENDL;
//	LOG_INFO << "Source (Tel62) sub-ID " << ((int) cedarHeader->sourceSubID) << ENDL;
//	LOG_INFO << "Format " << ((int) cedarHeader->format) << ENDL;

	for (int iFPGA = 0; iFPGA < maxNFPGA; iFPGA++) {
//		printf ("writing getpayload() + %d\n",1+iFPGA+nWords);
		fpgaHeader[iFPGA] = (FPGADataHeader*) payload + 1 + iFPGA + nWords;

		LOG_INFO<< "noFrame " << ((uint) fpgaHeader[iFPGA]->noFrame) << ENDL;
		LOG_INFO<< "noNonEmptyFrame " << ((uint) fpgaHeader[iFPGA]->noNonEmptyFrame) << ENDL;
		LOG_INFO<< "FPGAID[" << iFPGA<< "] " << ((uint) fpgaHeader[iFPGA]->FPGAID) << ENDL;
		LOG_INFO<< "errFlags[iFPGA] " << ((uint) fpgaHeader[iFPGA]->errFlags) << ENDL;

		noFrame[iFPGA] = (uint) fpgaHeader[iFPGA]->noFrame;
		noNonEmptyFrame[iFPGA] = (uint) fpgaHeader[iFPGA]->noNonEmptyFrame;
		FPGAID[iFPGA] = (uint) fpgaHeader[iFPGA]->FPGAID;
		errFlags[iFPGA] = (uint) fpgaHeader[iFPGA]->errFlags;

		printf("noFrame[%d] %d\n", iFPGA, noFrame[iFPGA]);
		printf("noNonEmptyFrame[%d] %d\n", iFPGA, noNonEmptyFrame[iFPGA]);
		printf("FPGAID[%d] %d\n", iFPGA, FPGAID[iFPGA]);
		printf("errFlags[%d] %d\n", iFPGA, errFlags[iFPGA]);

		for (int iFrame = 0; iFrame < maxNFrame; iFrame++) {
//			printf ("writing getpayload() + %d\n",2+iFPGA+nWords);
			frameHeader[iFPGA][iFrame] = (FrameDataHeader*) payload + 2 + iFPGA
					+ nWords;

			LOG_INFO<< "coarseFrameTime " << ((uint16_t) frameHeader[iFPGA][iFrame]->coarseFrameTime) << ENDL;
			LOG_INFO<< "nWordsPerFrame " << ((uint) frameHeader[iFPGA][iFrame]->nWordsPerFrame) << ENDL;

			coarseFrameTime[iFPGA][iFrame] =
					(uint16_t) frameHeader[iFPGA][iFrame]->coarseFrameTime;
			nWordsPerFrame[iFPGA][iFrame] =
					(uint) frameHeader[iFPGA][iFrame]->nWordsPerFrame;
//nWordsPerFPGA[iFPGA] += nWordsPerFrame[iFPGA][iFrame];
			nWords += (uint) frameHeader[iFPGA][iFrame]->nWordsPerFrame;

			printf("Frame TimeStamp[%d][%d] = %04x\n", iFPGA, iFrame,
					coarseFrameTime[iFPGA][iFrame]);
			printf("nWordsPerFrame[%d][%d] %d\n", iFPGA, iFrame,
					(uint) nWordsPerFrame[iFPGA][iFrame]);
			//printf("nWordsPerFPGA[%d] %d\n",iFPGA,(int)nWordsPerFPGA[iFPGA]);
//			printf("nWords %d\n", (int) nWords);

			if (nWordsPerFrame)
				nhits = nWordsPerFrame[iFPGA][iFrame] - 1;
			else
				LOG_INFO<< "CedarData.cpp: Number of Words in Frame is Null !" << ENDL;
			printf("nhits %d\n", nhits);
			if (nhits) {
				for (uint ihit = 0; ihit < nhits; ihit++) {
//					printf("ihit %d", ihit);
//					printf("................writing getpayload() + %d\n",
//							2 + iFPGA + nWords - nhits + ihit);
//					printf("tdc_data[%d]\n", ihit + nhits_tot);
					tdc_data[ihit + nhits_tot] = (TrbData*) payload + 2 + iFPGA
							+ nWords - nhits + ihit;
//					printf("tdc word %08x\n",(uint32_t) tdc_data[ihit + nhits_tot]->tdcWord);
					LOG_INFO<< "Time " << (uint32_t) tdc_data[ihit+nhits_tot]->Time << ENDL;
					LOG_INFO<< "ChID " << (uint) tdc_data[ihit+nhits_tot]->chID << ENDL;
					LOG_INFO<< "TDCID " << (uint) tdc_data[ihit+nhits_tot]->tdcID << ENDL;
					LOG_INFO<< "ID " << (uint) tdc_data[ihit+nhits_tot]->ID << ENDL;

					time[ihit + nhits_tot] = (uint32_t) tdc_data[ihit
							+ nhits_tot]->Time;
					chID[ihit + nhits_tot] =
							(uint) tdc_data[ihit + nhits_tot]->chID;
					tdcID[ihit + nhits_tot] =
							(uint) tdc_data[ihit + nhits_tot]->tdcID;
					ID[ihit + nhits_tot] =
							(uint) tdc_data[ihit + nhits_tot]->ID;

					printf("time[%d] %08x\n", ihit + nhits_tot,
							time[ihit + nhits_tot]);
					printf("chID[%d] %d\n", ihit + nhits_tot,
							chID[ihit + nhits_tot]);
					printf("tdcID[%d] %d\n", ihit + nhits_tot,
							tdcID[ihit + nhits_tot]);
					printf("ID[%d] %d\n", ihit + nhits_tot,
							ID[ihit + nhits_tot]);

					if (ihit == (nhits - 1)) {
						nhits_tot += nhits;
//						printf("nhits_tot %d\n", nhits_tot);
					}
				}
			}
		}
	}
}

}
