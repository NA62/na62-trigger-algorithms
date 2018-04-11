/*
 * RICHMultiTrackAlgo.h
 *
 *  Created on: 7 Aug 2017
 *      Author: romano
 */

#ifndef L1_RICHMULTITRACKALGO_H_
#define L1_RICHMULTITRACKALGO_H_

#include <sys/types.h>
#include <cstdint>

#include "../common/decoding/DecoderHandler.h"
#include "rich_algorithm/ParsConfFile.h"
#include "l1/L1InfoToStorage.h"
#include "../struct/HLTConfParams.h"
#include "L1Fragment.h"

#define maxNROchs 512

namespace na62 {

class RICHMultiTrackAlgo {
public:

	static uint_fast8_t processRICHMultiTrackTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info);
	static void initialize(uint i, l1RICH &l1RICHStruct);
	static void writeData(L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info);

private:

	static RICHParsConfFile* InfoRICH_;
	static uint AlgoID_; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
	static uint AlgoLogic_[16];
	static uint AlgoRefTimeSourceID_[16];
	static double AlgoOnlineTimeWindow_[16];

	static int* PMTGeo_;
};

} /* namespace na62 */

#endif /* L1_RICHMULTITRACKALGO_H_ */
