/*
 * KtagAlgo.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk 
 */

#include "KtagAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <options/Logging.h>
#include "data_decoder/TrbDecoder.h"
//#include "data_decoder/CedarHits.h"
#include "cedar_algorithm/tdcb_buffer.h"

namespace na62 {

KtagAlgo::KtagAlgo() {
}

KtagAlgo::~KtagAlgo() {
// TODO Auto-generated destructor stub
}

uint8_t KtagAlgo::checkKtagTrigger(Event* event) {

	using namespace l0;

	TrbDecoder* cedarPacket = new TrbDecoder[maxNTEL62s]; //max NTel62 boards

	uint noHitsPerTrb[maxNTEL62s];
	uint sector_occupancy[8];
	uint nSectors = 0;
	//CedarHits cedarHits[maxNhits];

	uint nWordsPerFPGA[maxNFPGAs];

	uint* noFrame = new uint[maxNFPGAs];
	uint* noNonEmptyFrame = new uint[maxNFPGAs];
	uint* FPGAID = new uint[maxNFPGAs];
	uint* errFlags = new uint[maxNFPGAs];

	uint16_t** coarseFrameTime = new uint16_t*[maxNFPGAs];
	uint** nWordsPerFrame = new uint*[maxNFPGAs];

	for (uint i = 0; i < maxNFPGAs; i++) {
		coarseFrameTime[i] = new uint16_t[maxNFrames];
		nWordsPerFrame[i] = new uint[maxNFrames];
	}

	uint32_t* time = new uint32_t[maxNhits];
	uint* chID = new uint[maxNhits];
	uint* tdcID = new uint[maxNhits];
	uint* ID = new uint[maxNhits];
	uint* trbID = new uint[maxNhits];

	uint pp[maxNhits];
	uint tdc[maxNhits];
	uint box[maxNhits];

	for (uint i = 0; i < maxNTEL62s; i++) {
		noHitsPerTrb[i] = 0;
	}
	for (uint i = 0; i < maxNhits; i++) {
		if (i < 8)
			sector_occupancy[i] = 0;
		time[i] = 0;
		chID[i] = 999;
		tdcID[i] = 999;
		ID[i] = 999;
		trbID[i] = 999;
		pp[i] = 999;
		tdc[i] = 999;
		box[i] = 999;
	}

	uint nWords = 0;
	uint nhits_tot = 0;
	uint nhits_tot_check = 0;

	for (uint iFPGA = 0; iFPGA < maxNFPGAs; iFPGA++) {
		nWordsPerFPGA[iFPGA] = 0;
	}

	uint chkmax = 0;

	//LOG_INFO<< "Event number = " << event->getEventNumber() << ENDL;
	//LOG_INFO<< "Timestamp = " << std::hex << event->getTimestamp() << std::dec << ENDL;

	l0::Subevent* cedarSubevent = event->getCEDARSubevent();

	for (uint trbNum = 0;
			trbNum != cedarSubevent->getNumberOfFragments() && chkmax == 0;
			trbNum++) {
		l0::MEPFragment* trbDataFragment = cedarSubevent->getFragment(trbNum);

		cedarPacket[trbNum].SetHits(trbNum, trbDataFragment);

		noFrame = cedarPacket[trbNum].GetNoFrame();
		noNonEmptyFrame = cedarPacket[trbNum].GetNoNonEmptyFrame();
		FPGAID = cedarPacket[trbNum].GetFPGAID();
		errFlags = cedarPacket[trbNum].GetErrFlags();
		coarseFrameTime = cedarPacket[trbNum].GetCoarseFrameTime();
		nWordsPerFrame = cedarPacket[trbNum].GetNWordsPerFrame();
		time = cedarPacket[trbNum].GetTime();
		chID = cedarPacket[trbNum].GetChID();
		tdcID = cedarPacket[trbNum].GetTdcID();
		ID = cedarPacket[trbNum].GetID();
		trbID = cedarPacket[trbNum].GetTrbID();
		noHitsPerTrb[trbNum] = cedarPacket[trbNum].GetNoHitsPerTrb();

		for (uint iFPGA = 0; iFPGA < maxNFPGAs; iFPGA++) {

//			printf("KtagAlgo.cpp: noFrame[%d] %d\n", iFPGA, noFrame[iFPGA]);
//			printf("KtagAlgo.cpp: noNonEmptyFrame[%d] %d\n", iFPGA,
//					noNonEmptyFrame[iFPGA]);
//			printf("KtagAlgo.cpp: FPGAID[%d] %d\n", iFPGA, FPGAID[iFPGA]);
//			printf("KtagAlgo.cpp: errFlags[%d] %d\n", iFPGA, errFlags[iFPGA]);

			for (uint iFrame = 0; iFrame < maxNFrames; iFrame++) {

//				printf("KtagAlgo.cpp: coarseFrameTime[%d][%d] %04x\n", iFPGA,
//						iFrame, coarseFrameTime[iFPGA][iFrame]);
//				printf("KtagAlgo.cpp: nWordsPerFrame[%d][%d] %d\n", iFPGA,
//						iFrame, nWordsPerFrame[iFPGA][iFrame]);

				coarseFrameTime[iFPGA][iFrame] += (event->getTimestamp()
						& 0xFFFF0000);
				if ((event->getTimestamp() & 0xf000) == 0xf000
						&& (coarseFrameTime[iFPGA][iFrame] & 0xf000) == 0x0000)
					coarseFrameTime[iFPGA][iFrame] += 0x10000; //16 bits overflow
				if ((event->getTimestamp() & 0xf000) == 0x0000
						&& (coarseFrameTime[iFPGA][iFrame] & 0xf000) == 0xf000)
					coarseFrameTime[iFPGA][iFrame] -= 0x10000; //16 bits overflow

				nWordsPerFPGA[iFPGA] += nWordsPerFrame[iFPGA][iFrame];
				nWords += nWordsPerFrame[iFPGA][iFrame];

				if (nWordsPerFrame[iFPGA][iFrame])
					nhits_tot_check += (nWordsPerFrame[iFPGA][iFrame] - 1);
				else
					LOG_INFO<< "KtagAlgo.cpp::nWordsPerFrame is zero!"<< ENDL;
				}
			}

		for (uint ihit = 0; ihit < noHitsPerTrb[trbNum]; ihit++) {
			if (ID[ihit]) {
				//LOG_INFO<< "Hit " << ihit + nhits_tot << " ID " << ID[ihit] << ENDL;
				//LOG_INFO << "Hit " << ihit + nhits_tot << " time " << std::hex << time[ihit] << std::dec << ENDL;
				//LOG_INFO << "Hit " << ihit + nhits_tot << " chID " << chID[ihit] << ENDL;
				//LOG_INFO << "Hit " << ihit + nhits_tot << " tdcID " << tdcID[ihit] << ENDL;
				//LOG_INFO << "Hit " << ihit + nhits_tot << " trbID " << trbID[ihit] << ENDL;

				pp[ihit + nhits_tot] = tdcID[ihit] / 4;
				tdc[ihit + nhits_tot] = tdcID[ihit] % 4;
				//LOG_INFO << "pp[" << ihit + nhits_tot << "] " << pp[ihit + nhits_tot] << ENDL;
				//LOG_INFO << "tdc[" << ihit + nhits_tot << "] " << tdc[ihit + nhits_tot] << ENDL;

				box[ihit + nhits_tot] = searchPMT(trbNum, pp[ihit + nhits_tot]);
				//LOG_INFO << "box[" << ihit + nhits_tot << "] " << box[ihit + nhits_tot] << ENDL;

				sector_occupancy[box[ihit + nhits_tot] - 1]++;

				//LOG_INFO << event->getEventNumber() << "\t" << event->getTimestamp() << "\t" << ID[ihit] << "\t" << chID[ihit] << "\t" << tdcID[ihit] << "\t" << box[ihit+nhits_tot] << ENDL;

			}
			if (ihit == (noHitsPerTrb[trbNum] - 1)) {
				nhits_tot += noHitsPerTrb[trbNum];
			}
		}
	}

	for (int iSec = 0; iSec < 8; iSec++) {
		if (sector_occupancy[iSec])
			nSectors++;
	}

	LOG_INFO << event->getEventNumber() << "\t" << std::hex << event->getTimestamp() << std::dec <<  "\t" << nSectors << ENDL;

	uint8_t kaontrigger = 0;

	if (nSectors > 3) {
		kaontrigger = 1;
		return kaontrigger;
	} else
		return 0;

}

uint KtagAlgo::searchPMT(uint tel62ID, uint fpgaID) {
	uint sectorID;
	if ((tel62ID == 0) && ((fpgaID == 0) || (fpgaID == 1) || (fpgaID == 2)))
		sectorID = 1;
	else if ((tel62ID == 0) && (fpgaID == 3))
		sectorID = 2;
	else if ((tel62ID == 1) && ((fpgaID == 0) || (fpgaID == 1)))
		sectorID = 2;
	else if ((tel62ID == 1) && ((fpgaID == 2) || (fpgaID == 3)))
		sectorID = 3;
	else if ((tel62ID == 2) && (fpgaID == 0))
		sectorID = 3;
	else if ((tel62ID == 2)
			&& ((fpgaID == 1) || (fpgaID == 2) || (fpgaID == 3)))
		sectorID = 4;
	else if ((tel62ID == 3)
			&& ((fpgaID == 0) || (fpgaID == 1) || (fpgaID == 2)))
		sectorID = 5;
	else if ((tel62ID == 3) && (fpgaID == 3))
		sectorID = 6;
	else if ((tel62ID == 4) && ((fpgaID == 0) || (fpgaID == 1)))
		sectorID = 6;
	else if ((tel62ID == 4) && ((fpgaID == 2) || (fpgaID == 3)))
		sectorID = 7;
	else if ((tel62ID == 5) && (fpgaID == 0))
		sectorID = 7;
	else if ((tel62ID == 5)
			&& ((fpgaID == 1) || (fpgaID == 2) || (fpgaID == 3)))
		sectorID = 8;
	else
		printf("Combination KTAG %d, PP %d not found \n", tel62ID, fpgaID);
	return sectorID;
}

} /* namespace na62 */

