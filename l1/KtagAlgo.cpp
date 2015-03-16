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
#include <string.h>

#define maxNhits 500

namespace na62 {

KtagAlgo::KtagAlgo() {
}

KtagAlgo::~KtagAlgo() {
// TODO Auto-generated destructor stub
}

uint_fast8_t KtagAlgo::processKtagTrigger(Event* event) {

	using namespace l0;

	l0::Subevent* cedarSubevent = event->getCEDARSubevent();

	const uint nTEL62s = cedarSubevent->getNumberOfFragments();

	TrbDecoder *cedarPacket = new TrbDecoder[nTEL62s]; //max NTel62 boards

	uint noEdgesPerTrb[nTEL62s];
	memset(noEdgesPerTrb, 0, nTEL62s);

	uint sector_occupancy[8];
	memset(sector_occupancy, 0, 8);

	uint64_t* edge_times;
	uint* edge_chIDs;
	uint* edge_tdcIDs;
	uint* edge_IDs;
	uint* edge_trbIDs;

	uint pp[maxNhits];
	uint tdc[maxNhits];
	uint box[maxNhits];

	memset(pp,999,maxNhits);
	memset(tdc,999,maxNhits);
	memset(box,999,maxNhits);

	uint nEdges_tot = 0;
	uint chkmax = 0;

	//LOG_INFO<< "Event number = " << event->getEventNumber() << ENDL;
	//LOG_INFO<< "Timestamp = " << std::hex << event->getTimestamp() << std::dec << ENDL;

	//TODO: chkmax need to be USED

	for (uint trbNum = 0; trbNum != nTEL62s && chkmax == 0; trbNum++) {

		l0::MEPFragment* trbDataFragment = cedarSubevent->getFragment(trbNum);

		cedarPacket[trbNum].GetData(trbNum, trbDataFragment, event->getTimestamp());

		/**
		 * Get Arrays with hit Info
		 *
		 */
		edge_times = cedarPacket[trbNum].GetTimes();
		edge_chIDs = cedarPacket[trbNum].GetChIDs();
		edge_tdcIDs = cedarPacket[trbNum].GetTdcIDs();
		edge_IDs = cedarPacket[trbNum].GetIDs();
		edge_trbIDs = cedarPacket[trbNum].GetTrbIDs();

		noEdgesPerTrb[trbNum] = cedarPacket[trbNum].GetNoEdgesPerTrb();

		//LOG_INFO<< "Tel62 ID " << trbNum << " - Number of Edges found " << noEdgesPerTrb[trbNum] << ENDL;

		for (uint iEdge = 0; iEdge != noEdgesPerTrb[trbNum]; iEdge++) {
			if (edge_IDs[iEdge]) {

				//LOG_INFO<< "Edge " << iEdge + nEdges_tot << " ID " << edge_IDs[iEdge] << ENDL;
				//LOG_INFO<< "Edge " << iEdge + nEdges_tot << " chID " << edge_chIDs[iEdge] << ENDL;
				//LOG_INFO<< "Edge " << iEdge + nEdges_tot << " tdcID " << edge_tdcIDs[iEdge] << ENDL;
				//LOG_INFO<< "Edge " << iEdge + nEdges_tot << " time " << std::hex << edge_times[iEdge] << std::dec << ENDL;
				//LOG_INFO<< "Edge " << iEdge + nEdges_tot << " trbID " << edge_trbIDs[iEdge] << ENDL;

				pp[iEdge + nEdges_tot] = edge_tdcIDs[iEdge] / 4;
				//LOG_INFO<< "pp[" << iEdge + nEdges_tot << "] " << pp[iEdge + nEdges_tot] << ENDL;

				box[iEdge + nEdges_tot] = searchPMT(trbNum,
						pp[iEdge + nEdges_tot]);
				//LOG_INFO << "box[" << iEdge + nEdges_tot << "] " << box[iEdge + nEdges_tot] << ENDL;

				sector_occupancy[box[iEdge + nEdges_tot] - 1]++;

				//LOG_INFO<< "ANGELA-L1" << "\t" << event->getEventNumber() << "\t" << event->getTimestamp() << "\t" << edge_IDs[iEdge] << "\t" << edge_chIDs[iEdge]<< "\t" << edge_tdcIDs[iEdge] << "\t" << edge_times[iEdge] << "\t" << edge_trbIDs[iEdge] << "\t" << box[iEdge+nEdges_tot] << ENDL;

			}
			if (iEdge == (noEdgesPerTrb[trbNum] - 1)) {
				nEdges_tot += noEdgesPerTrb[trbNum];
			}
		}
	}

	//LOG_INFO<<"KtagAlgo.cpp: Analysing Event " << event->getEventNumber() << " - Total Number of edges found " << nEdges_tot << ENDL;

	uint nSectors = 0;
	for (int iSec = 0; iSec < 8; iSec++) {
		if (sector_occupancy[iSec])
			nSectors++;
	}

	//LOG_INFO<< "Angela: " << event->getEventNumber() << "\t" << event->getTimestamp() << "\t" << nSectors << ENDL;

	delete [] cedarPacket;

	uint_fast8_t kaontrigger = 0;

	if (nSectors > 3) {
		kaontrigger = 1;
		return kaontrigger;
	} else
		return 0;

}

/*
 * TODO: try to remove ifs: options are an algorithm or a look-up table
 *
 */
uint KtagAlgo::searchPMT(uint tel62ID, uint fpgaID) {
	uint sectorID = 0;
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
		LOG_ERROR<< "Combination KTAG Tel62 ID " << tel62ID << " and PP " << fpgaID << " not found !" << ENDL;
	return sectorID;
}

} /* namespace na62 */

