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

KtagAlgo::KtagAlgo() {}

KtagAlgo::~KtagAlgo() {
// TODO Auto-generated destructor stub
}

uint8_t KtagAlgo::checkKtagTrigger(Event* event) {

	using namespace l0;

	TrbDecoder* cedarPacket = new TrbDecoder[32];

	int nhits_perTrb[32];
	int sector_occupancy[8];
	int nSectors = 0;
	//CedarHits cedarHits[maxNhit];

	uint nWordsPerFPGA[maxNFPGA];

	uint* noFrame = new uint[maxNFPGA];
	uint* noNonEmptyFrame = new uint[maxNFPGA];
	uint* FPGAID = new uint[maxNFPGA];
	uint* errFlags = new uint[maxNFPGA];

	uint16_t** coarseFrameTime = new uint16_t*[maxNFPGA];
	uint** nWordsPerFrame = new uint*[maxNFPGA];

	for (int i = 0; i < maxNFPGA; i++) {
		coarseFrameTime[i] = new uint16_t[maxNFrame];
		nWordsPerFrame[i] = new uint[maxNFrame];
	}

	uint32_t* time = new uint32_t[maxNhit];
	uint* chID = new uint[maxNhit];
	uint* tdcID = new uint[maxNhit];
	uint* ID = new uint[maxNhit];

	int pp[maxNhit];
	int tdc[maxNhit];
	int box[maxNhit];

	for (int i = 0; i < maxNhit; i++) {
		if (i < 8)
			sector_occupancy[i] = 0;
		time[i] = 0;
		chID[i] = 0;
		tdcID[i] = 0;
		ID[i] = 0;
		pp[i] = -1;
		tdc[i] = -1;
		box[i] = -1;
	}

	uint nWords = 0;
	uint nhits = 0;
	uint nhits_tot = 0;
	uint nhits_tot_check = 0;

	for (int iFPGA = 0; iFPGA < maxNFPGA; iFPGA++) {
		nWordsPerFPGA[iFPGA] = 0;
	}

	uint chkmax = 0;

//	LOG_INFO << ENDL;
//	LOG_INFO << "Event number = " << event->getEventNumber() << ENDL;
	printf("timestamp = %x\n", event->getTimestamp());

	l0::Subevent* cedarSubevent = event->getCEDARSubevent();

	for (int trbNum = 0;
			trbNum != cedarSubevent->getNumberOfFragments() && chkmax == 0;
			trbNum++) {
		l0::MEPFragment* trbDataFragment = cedarSubevent->getFragment(trbNum);

		cedarPacket[trbNum].SetNoFrame(noFrame);
		cedarPacket[trbNum].SetNoNonEmptyFrame(noNonEmptyFrame);
		cedarPacket[trbNum].SetFPGAID(FPGAID);
		cedarPacket[trbNum].SetErrFlags(errFlags);
		cedarPacket[trbNum].SetCoarseFrameTime(coarseFrameTime);
		cedarPacket[trbNum].SetNWordsPerFrame(nWordsPerFrame);
		cedarPacket[trbNum].SetTime(time);
		cedarPacket[trbNum].SetChID(chID);
		cedarPacket[trbNum].SetTdcID(tdcID);
		cedarPacket[trbNum].SetID(ID);

		cedarPacket[trbNum].SetHits(trbDataFragment);

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

		nhits_perTrb[trbNum] = 0;

		for (int iFPGA = 0; iFPGA < maxNFPGA; iFPGA++) {
//			noFrame[iFPGA] = (uint8_t) (cedarPacket[trbNum].fpgaHeader[iFPGA]->noFrame);
//			noNonEmptyFrame[iFPGA] = (uint8_t)cedarPacket[trbNum].fpgaHeader[iFPGA]->noNonEmptyFrame;
//			FPGAID[iFPGA] = (uint8_t) cedarPacket[trbNum].fpgaHeader[iFPGA]->FPGAID;
//			errFlags[iFPGA] = (uint8_t) cedarPacket[trbNum].fpgaHeader[iFPGA]->errFlags;

			printf("KtagAlgo.cpp: noFrame[%d] %d\n", iFPGA, noFrame[iFPGA]);
			printf("KtagAlgo.cpp: noNonEmptyFrame[%d] %d\n", iFPGA,noNonEmptyFrame[iFPGA]);
			printf("KtagAlgo.cpp: FPGAID[%d] %d\n", iFPGA, FPGAID[iFPGA]);
			printf("KtagAlgo.cpp: errFlags[%d] %d\n", iFPGA, errFlags[iFPGA]);

			for (int iFrame = 0; iFrame < maxNFrame; iFrame++) {

				printf("KtagAlgo.cpp: coarseFrameTime[%d][%d] %04x\n", iFPGA,
						iFrame, coarseFrameTime[iFPGA][iFrame]);
				printf("KtagAlgo.cpp: nWordsPerFrame[%d][%d] %d\n", iFPGA,
						iFrame, nWordsPerFrame[iFPGA][iFrame]);

//				coarseFrameTime[iFPGA][iFrame] =
//					(uint16_t) cedarPacket[trbNum].frameHeader[iFPGA][iFrame]->coarseFrameTime;
//				nWordsPerFrame[iFPGA][iFrame] =
//						(uint16_t) cedarPacket[trbNum].frameHeader[iFPGA][iFrame]->nWordsPerFrame;

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

				nhits = nWordsPerFrame[iFPGA][iFrame] - 1;
				if (nhits) {
					for (uint ihit = 0; ihit < nhits; ihit++) {
//						time[ihit + nhits_tot] =
//								(uint32_t) cedarPacket[trbNum].tdc_data[ihit
//										+ nhits_perTrb[trbNum]]->Time;
//						chID[ihit + nhits_tot] =
//								(uint8_t) cedarPacket[trbNum].tdc_data[ihit
//										+ nhits_perTrb[trbNum]]->chID;
//						tdcID[ihit + nhits_tot] =
//								(uint8_t) cedarPacket[trbNum].tdc_data[ihit
//										+ nhits_perTrb[trbNum]]->tdcID;
//						ID[ihit + nhits_tot] =
//								(uint8_t) cedarPacket[trbNum].tdc_data[ihit
//										+ nhits_perTrb[trbNum]]->ID;
//						printf("tdc word %08x\n",(uint32_t) cedarPacket[trbNum].tdc_data[ihit+ nhits_perTrb[trbNum]]->tdcWord);
						if (ID[ihit + nhits_tot]) {
							printf("ID[%d] %x \n", ihit + nhits_tot,
									ID[ihit + nhits_tot]);
							printf("Time[%d] %08x \n", ihit + nhits_tot,
									time[ihit + nhits_tot]);
							printf("ChID[%d] %x \n", ihit + nhits_tot,
									chID[ihit + nhits_tot]);
							printf("TDCID[%d] %x \n", ihit + nhits_tot,
									tdcID[ihit + nhits_tot]);
							printf("TrbNum %d\n", trbNum);
						}
						if (ihit == (nhits - 1)) {
							nhits_tot += nhits;
							nhits_perTrb[trbNum] += nhits;
						}
					}
				}
			}
		}

		for (uint ihit = nhits_tot_check; ihit < maxNhit; ihit++) {
			if (ID[ihit]) {
//				printf("hit[%d] ID %d\n", ihit, (uint) ID[ihit]);
//				printf("hit[%d] time %08x\n", ihit, (uint32_t) time[ihit]);
//				printf("hit[%d] chID %d\n", ihit, (uint) chID[ihit]);
//				printf("hit[%d] tdcID %x\n", ihit, (uint8_t) tdcID[ihit]);
				pp[ihit] = tdcID[ihit] / 4;
				tdc[ihit] = tdcID[ihit] % 4;
//				printf("pp[%d] %d\n", ihit, pp[ihit]);
//				printf("tdc[%d] %d\n", ihit, tdc[ihit]);

				box[ihit] = searchPMT(trbNum, pp[ihit]);

//				printf("Found Sector %d\n", box[ihit]);

				sector_occupancy[box[ihit] - 1]++;
				nhits_tot_check++;

				printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
						event->getEventNumber(), event->getTimestamp(),
						ID[ihit], trbNum, tdcID[ihit], pp[ihit], chID[ihit],
						box[ihit]);

			}
		}
	}

	for (int iSec = 0; iSec < 8; iSec++) {
		if (sector_occupancy[iSec])
			nSectors++;
	}

	std::cout << "NSectors " << nSectors << std::endl;
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

