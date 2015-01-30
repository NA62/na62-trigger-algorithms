/*
 * CedarData.cpp
 *
 *  Created on: Nov 24, 2014
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

#include "CedarData.h"
#include "../cedar_algorithm/tdcb_buffer.h"
#include <options/Logging.h>
#include <eventBuilding/Event.h>
#include <l0/MEPFragment.h>
#include <stdlib.h>
#include <cstdint>

namespace na62 {

CedarData::CedarData() {
	nhits = 0;
	nhits_tot = 0;
	nWords = 0;
	cedarHeader = 0;
	cedar_fpgaHeader = new FPGADataHeader*[maxNFPGA];
	cedar_frameHeader = new FrameDataHeader**[maxNFPGA];
	tdc_data = new TrbData*[maxNhit];
	for (int i = 0; i < maxNFPGA; i++) {
		cedar_frameHeader[i] = new FrameDataHeader*[maxNFrame];
	}
}

CedarData::~CedarData() {
	// TODO Auto-generated destructor stub
}

void CedarData::SetHits(l0::MEPFragment* trbDataFragment) {
	char * payload = trbDataFragment->getPayload();
	cedarHeader = (TrbDataHeader*) payload;

//	LOG_INFO << "maxNFPGA " << maxNFPGA << ENDL;
//	LOG_INFO << "maxNFrame " << maxNFrame << ENDL;
//
//	LOG_INFO << "Flags " << ((int) cedarHeader->flags) << ENDL;
//	LOG_INFO << "L0 trigger type " << ((int) cedarHeader->triggerType) << ENDL;
//	LOG_INFO << "Source (Tel62) sub-ID " << ((int) cedarHeader->sourceSubID) << ENDL;
//	LOG_INFO << "Format " << ((int) cedarHeader->format) << ENDL;

	for (int iFPGA = 0; iFPGA < maxNFPGA; iFPGA++) {
//		printf ("writing getpayload() + %d\n",1+iFPGA+nWords);
		cedar_fpgaHeader[iFPGA] = (FPGADataHeader*) payload + 1 + iFPGA
				+ nWords;

//		LOG_INFO << "noFrame " << ((int) cedar_fpgaHeader[iFPGA]->noFrame) << ENDL;
//		LOG_INFO << "noNonEmptyFrame " << ((int) cedar_fpgaHeader[iFPGA]->noNonEmptyFrame) << ENDL;
//		LOG_INFO << "FPGAID[" << iFPGA<< "] " << ((int) cedar_fpgaHeader[iFPGA]->FPGAID) << ENDL;
//		LOG_INFO << "errFlags[iFPGA] " << ((int) cedar_fpgaHeader[iFPGA]->errFlags) << ENDL;

//noFrame[iFPGA] = (uint8_t)cedar_fpgaHeader[iFPGA]->noFrame;
//noNonEmptyFrame[iFPGA] = (uint8_t)cedar_fpgaHeader[iFPGA]->noNonEmptyFrame;
//FPGAID[iFPGA] = (uint8_t) cedar_fpgaHeader[iFPGA]->FPGAID;
//errFlags[iFPGA] = (uint8_t) cedar_fpgaHeader[iFPGA]->errFlags;

//printf("noFrame[%d] %08x\n",iFPGA,noFrame[iFPGA]);
//printf("noNonEmptyFrame[%d] %08x\n",iFPGA,noNonEmptyFrame[iFPGA]);
//printf("FPGAID[%d] %08x\n",iFPGA,FPGAID[iFPGA]);
//printf("errFlags[%d] %08x\n",iFPGA,errFlags[iFPGA]);

		for (int iFrame = 0; iFrame < maxNFrame; iFrame++) {
//			printf ("writing getpayload() + %d\n",2+iFPGA+nWords);
			cedar_frameHeader[iFPGA][iFrame] = (FrameDataHeader*) payload + 2
					+ iFPGA + nWords;

//			LOG_INFO << "coarseFrameTime " << ((uint16_t) cedar_frameHeader[iFPGA][iFrame]->coarseFrameTime) << ENDL;
//			LOG_INFO << "nWordsPerFrame " << ((uint16_t) cedar_frameHeader[iFPGA][iFrame]->nWordsPerFrame) << ENDL;

			//coarseFrameTime[iFPGA][iFrame] = (uint16_t) cedar_frameHeader[iFPGA][iFrame]->coarseFrameTime;
			//nWordsPerFrame[iFPGA][iFrame] = (uint16_t) cedar_frameHeader[iFPGA][iFrame]->nWordsPerFrame;
			//nWordsPerFPGA[iFPGA] += nWordsPerFrame[iFPGA][iFrame];
			nWords += (uint) cedar_frameHeader[iFPGA][iFrame]->nWordsPerFrame;

			//printf("Frame TimeStamp[%d][%d] = %04x\n",iFPGA,iFrame,coarseFrameTime[iFPGA][iFrame]);
			//printf("nWordsPerFrame[%d][%d] %d\n",iFPGA,iFrame,(int)nWordsPerFrame[iFPGA][iFrame]);
			//printf("nWordsPerFPGA[%d] %d\n",iFPGA,(int)nWordsPerFPGA[iFPGA]);
//			printf("nWords %d\n", (int) nWords);

			nhits = ((uint) cedar_frameHeader[iFPGA][iFrame]->nWordsPerFrame)
					- 1;
//			printf("nhits %d\n",nhits);
			if (nhits) {
//				printf("Am I entering here??? ..... nhits %d\n", nhits);
				for (uint ihit = 0; ihit < nhits; ihit++) {
//					printf("ihit %d", ihit);
//					printf("................writing getpayload() + %d\n",
//							2 + iFPGA + nWords - nhits + ihit);
//					printf("tdc_data[%d]\n", ihit + nhits_tot);
					tdc_data[ihit + nhits_tot] = (TrbData*) payload + 2 + iFPGA
							+ nWords - nhits + ihit;
//					printf("tdc word %08x\n",(uint32_t) tdc_data[ihit + nhits_tot]->tdcWord);
//					LOG_INFO << "Time " << (uint32_t) tdc_data[ihit+nhits_tot]->Time << ENDL;
//					LOG_INFO << "ChID " << (uint8_t) tdc_data[ihit+nhits_tot]->chID << ENDL;
//					LOG_INFO << "TDCID " << (uint8_t) tdc_data[ihit+nhits_tot]->tdcID << ENDL;
//					LOG_INFO << "ID "    << (uint8_t) tdc_data[ihit+nhits_tot]->ID << ENDL;
					if (ihit == (nhits - 1)) {
						nhits_tot += nhits;
//						printf("nhits_tot %d\n", nhits_tot);
					}
				}
			}
		}
	}
}

//uint8_t* CedarData::GetNoFrame(){
//	return noFrame;
//}
//
//uint8_t* CedarData::GetNoNonEmptyFrame(){
//  	return noNonEmptyFrame;
//  }
//
//uint8_t* CedarData::GetFPGAID(){
//  	return FPGAID;
//  }
//
//uint8_t* CedarData::GetErrFlags(){
//  	return errFlags;
//  }
//
//uint16_t** CedarData::GetCoarseFrameTime(){
//  	return coarseFrameTime;
//  }
//
//uint16_t** CedarData::GetNWordsPerFrame(){
//  	return nWordsPerFrame;
//  }

}
