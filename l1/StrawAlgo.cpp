/*
 * StrawAlgo.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk 
 */

#include "StrawAlgo.h"

#include <eventBuilding/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>
#include <options/Logging.h>
#include <string.h>
#include <math.h>

#include <sys/time.h>

#include "../common/decoding/DecoderRange.h"
#include "../common/decoding/DecoderHandler.h"
#include "../common/decoding/SrbFragmentDecoder.h"

#define maxNhits 500

namespace na62 {

uint_fast8_t StrawAlgo::processStrawTrigger(DecoderHandler& decoder) {

//  LOG_INFO<< "Initial Time " << time[0].tv_sec << " " << time[0].tv_usec << ENDL;

	using namespace l0;

//	double cedarOffsetFinetime = -10.; //ns (from run 3015)
//TODO: chkmax need to be USED
    LOG_INFO<< "receiving straw packet:" << endl;
	for (SrbFragmentDecoder* strawPacket : decoder.getSTRAWDecoderRange()) {

		const uint64_t* straw_id = strawPacket->getStrawIDs();
		const double* time = strawPacket->getTimes();
		const bool* isLeading = strawPacket->getIsLeadings();
		const uint nEdges = strawPacket->getNumberOfEdgesStored();
		const uint64_t* srbID = strawPacket->getSrbIDs();
        /*
		for (int i = 0; i < nEdges; i++){
		  LOG_INFO << srbID[i] << " " << straw_id[i] << " " << time[i] << " " << isLeading[i] << ENDL;
		}
		*/

	}

	return 0;
}

} /* namespace na62 */

