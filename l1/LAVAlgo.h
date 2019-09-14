/*
 * LAVAlgo.h
 *
 *  Created on: 7 Sep 2015
 *      Author: romano
 */

#pragma once
#ifndef L1_LAVALGO_H_
#define L1_LAVALGO_H_

#include <sys/types.h>
#include <cstdint>

#ifndef ONLINEHLT
#include <common/decoding/DecoderHandler.h>
#else
#include <common/decoding/OfflineDecoderHandler.h>
#endif

#include <l1/L1InfoToStorage.h>
#include <l1/L1Fragment.h>
#include <l1/lav_algorithm/LAVParsConfFile.h>
#include <struct/HLTConfParams.h>

#define maxNROchs 512

namespace na62 {

class LAVAlgo {
public:
	/**
	 * @param event Event* This is a pointer to the built Event containing all subevents (except those from the LKr)
	 *
	 * The event pointer may not be deleted by this method as it is deleted by the EventBuilder
	 *
	 * @return uint_fast8_t <0> if the event is rejected, the L1 trigger type word in other cases.
	 */

	static uint_fast8_t processLAVTrigger(uint l0MaskID, DecoderHandler& decoder, L1InfoToStorage* l1Info);
	static void initialize(uint i, l1LAV &l1LAVStruct);
	static void loadConfigurationFile(std::string absolute_chMapFile_path);
	static void writeData(L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info);

private:

	static LAVParsConfFile* InfoLAV_;
	static uint AlgoID_; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for IRCSAC, 5 for Straw, 6 for MUV3, 7 for NewCHOD
	static uint AlgoLogic_[16];
	static uint AlgoRefTimeSourceID_[16];
	static double AlgoOnlineTimeWindow_[16];

};

} /* namespace na62 */

#endif /* L1_LAVALGO_H_ */
