/*
 * MultiDetAlgo.cpp
 *
 *  Created on: March 5, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk 
 */

#include "MultiDetAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <options/Logging.h>
#include "data_decoder/TrbDecoder.h"

namespace na62 {

MultiDetAlgo::MultiDetAlgo() {
}

MultiDetAlgo::~MultiDetAlgo() {
// TODO Auto-generated destructor stub
}

uint8_t MultiDetAlgo::checkMultiDetTrigger(Event* event) {

	using namespace l0;

	l0::Subevent* cedarSubevent = event->getCEDARSubevent();
	l0::Subevent* chodSubevent = event->getCHODSubevent();

	uint nTEL62s_cedar = cedarSubevent->getNumberOfFragments();
	uint nTEL62s_chod = chodSubevent->getNumberOfFragments();

	TrbDecoder* cedarPacket = new TrbDecoder[nTEL62s_cedar];
	TrbDecoder* chodPacket = new TrbDecoder[nTEL62s_chod];

	uint noEdgesPerTrb_cedar[nTEL62s_cedar];
	uint noEdgesPerTrb_chod[nTEL62s_chod];

	double* edge_times;
	uint* edge_chIDs;
	uint* edge_tdcIDs;
	uint* edge_IDs;
	uint* edge_trbIDs;

	for (uint i = 0; i < nTEL62s_cedar; i++) {
		noEdgesPerTrb_cedar[i] = 0;
	}
	for (uint i = 0; i < nTEL62s_chod; i++) {
		noEdgesPerTrb_chod[i] = 0;
	}

	uint nEdges_cedar_tot = 0;
	uint nEdges_chod_tot = 0;

	uint chkmax = 0;

//	LOG_INFO<< "Event number = " << event->getEventNumber() << ENDL;
	LOG_INFO<< "Timestamp = " << std::hex << event->getTimestamp() << std::dec << ENDL;

	for (uint trbNum = 0; trbNum != nTEL62s_cedar && chkmax == 0; trbNum++) {

		l0::MEPFragment* trbDataFragment = cedarSubevent->getFragment(trbNum);
		cedarPacket[trbNum].GetData(trbNum, trbDataFragment, event);

		noEdgesPerTrb_cedar[trbNum] = cedarPacket[trbNum].GetNoEdgesPerTrb();
//		LOG_INFO<< "KTAG: Tel62 ID " << trbNum << " - Number of Edges found " << noEdgesPerTrb_cedar[trbNum] << ENDL;

		nEdges_cedar_tot += noEdgesPerTrb_cedar[trbNum];
	}

	for (uint trbNum = 0; trbNum != nTEL62s_chod && chkmax == 0; trbNum++) {

		l0::MEPFragment* trbDataFragment = chodSubevent->getFragment(trbNum);
		chodPacket[trbNum].GetData(trbNum, trbDataFragment, event);

		/**
		 * Get Arrays with hit Info
		 *
		 */
		edge_times = chodPacket[trbNum].GetTimes();
		edge_chIDs = chodPacket[trbNum].GetChIDs();
		edge_tdcIDs = chodPacket[trbNum].GetTdcIDs();
		edge_IDs = chodPacket[trbNum].GetIDs();
		edge_trbIDs = chodPacket[trbNum].GetTrbIDs();

		noEdgesPerTrb_chod[trbNum] = chodPacket[trbNum].GetNoEdgesPerTrb();
//		LOG_INFO<< "CHOD: Tel62 ID " << trbNum << " - Number of Edges found " << noEdgesPerTrb_chod[trbNum] << ENDL;

		nEdges_chod_tot += noEdgesPerTrb_chod[trbNum];
	}

	LOG_INFO<<"MultiDetAlgo.cpp: Analysed Event " << event->getEventNumber() << " - Total Number of edges found (KTAG) " << nEdges_cedar_tot << ENDL;
	LOG_INFO<<"MultiDetAlgo.cpp: Analysed Event " << event->getEventNumber() << " - Total Number of edges found (CHOD) " << nEdges_chod_tot << ENDL;

	uint8_t multi_det_trigger = 0;

	if (nEdges_cedar_tot > 3 && nEdges_chod_tot > 10) {
		return multi_det_trigger = 1;
	} else
		return 0;

}


} /* namespace na62 */

