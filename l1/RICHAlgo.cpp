/*
 *
 RICHAlgo.cpp
 *
 *  Created on: 17 Apr 2015
 *      Author: Valeria Fascianelli
 */

#include "RICHAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <options/Logging.h>
#include <string.h>
#include <vector>

#include "../common/decoding/DecoderRange.h"
#include "../common/decoding/DecoderHandler.h"
#include "../common/decoding/TrbFragmentDecoder.h"
#include "rich_algorithm/ParsConfFile.h"

#define maxNhits 500

namespace na62 {

uint_fast8_t RICHAlgo::processRICHTrigger(DecoderHandler& decoder) {

	using namespace l0;

	LOG_INFO<<"===== RICHAlgo.cpp: Analysing Event ===" << decoder.getDecodedEvent()->getEventNumber() << ENDL;

    //ParsConfFile::ParsConfFile();
    //vector<int> geoMap = ParsConfFile::getChannelMap();

	ParsConfFile prova;
    vector <int> chGeos = prova.getChannelMap();

	LOG_INFO<< "Channel Remap " << chGeos.at(1)<<ENDL;

	uint nEdges_tot = 0;
	uint* chRO = new uint[maxNhits];   // to be deleted

	for (TrbFragmentDecoder* richPacket : decoder.getRICHDecoderRange()) {

		LOG_INFO<< "Number of RICH Tel62s = " << decoder.getNumberOfRICHFragments() << ENDL;

		LOG_INFO<< "TEL62 ID = " << richPacket->getFragmentNumber() << ENDL;

		/**
		 * Get Arrays with hit Info
		 */

		const uint64_t* const edge_times = richPacket->getTimes();
		const uint_fast8_t* const edge_chIDs = richPacket->getChIDs();
		const bool* const edge_IDs = richPacket->getIsLeadings();
		const uint_fast8_t* const edge_tdcIDs = richPacket->getTdcIDs();
		const uint_fast16_t edge_trbIDs = richPacket->getFragmentNumber();

		uint numberOfEdgesOfCurrentBoard = richPacket->getNumberOfEdgesStored();

		for (uint iEdge = 0; iEdge != numberOfEdgesOfCurrentBoard; iEdge++) {

			LOG_INFO<< "Edge " << iEdge + nEdges_tot << " ID " << (uint)edge_IDs[iEdge] << ENDL;
			LOG_INFO<< "Edge " << iEdge + nEdges_tot << " chID " << (uint)edge_chIDs[iEdge] << ENDL;
			LOG_INFO<< "Edge " << iEdge + nEdges_tot << " tdcID " << (uint)edge_tdcIDs[iEdge] << ENDL;
			LOG_INFO<< "Edge " << iEdge + nEdges_tot << " trbID " << (uint)edge_trbIDs << ENDL;
//			//LOG_INFO<< "Edge " << iEdge + nEdges_tot << " time " << std::hex << edge_times[iEdge] << std::dec << ENDL;

			chRO[iEdge] = edge_trbIDs * 512 + edge_tdcIDs[iEdge] * 32
					+ edge_chIDs[iEdge];

			//LOG_INFO << "chRO " << chRO[iEdge] <<" chGEO " << handlerChMap.getChannelRemap(chRO[iEdge]) << ENDL;

			//const uint box = calculateSector(cedarPacket->getFragmentNumber(),
			//	trbID);
			//LOG_INFO << "box[" << iEdge + nEdges_tot << "] " << box[iEdge + nEdges_tot] << ENDL;

			//sector_occupancy[box]++;

		}
		//LOG_INFO<< "Number of edges of current board " << numberOfEdgesOfCurrentBoard << ENDL;

		nEdges_tot += numberOfEdgesOfCurrentBoard;
	}
	//LOG_INFO<< " ----- Total Number of edges found -----  " << nEdges_tot << ENDL;

	uint nRings = 0;
//	for (uint iSec = 0; iSec != 8; iSec++) {
//		if (sector_occupancy[iSec])
//			nSectors++;
//	}

	//LOG_INFO<< "Angela: " << decoder.getDecodedEvent()->getEventNumber() << "\t" << decoder.getDecodedEvent()->getTimestamp() << "\t" << nSectors << ENDL;

	return nRings;
}

}
/* namespace na62 */

