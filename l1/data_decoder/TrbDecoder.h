/*
 * TrbDecoder.h
 *
 *  Created on: Nov 24, 2014
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

#ifndef TRBDECODER_H_
#define TRBDECODER_H_

#include <sys/types.h>
#include <cstdint>
#include <l0/MEPFragment.h>

namespace na62 {

/**
 * struct containing Tel62 board header
 */
struct TrbDataHeader
{
	uint8_t fpgaFlags :8;
	uint8_t triggerType :8;
	uint8_t sourceSubID :8; //Tel62 readout board ID
	uint8_t format :8;
}__attribute__ ((__packed__));

/**
 * struct containing FPGA header
 */
struct FPGADataHeader
{
	uint noFrames :8;
	uint noNonEmptyFrames :8;
	uint FPGAID :8;
	uint errFlags :8;
}__attribute__ ((__packed__));

/**
 * struct containing Frame header
 */
struct FrameDataHeader //Frame header
{
	uint16_t frameTimeStamp :16;
	uint nWords :16;
}__attribute__ ((__packed__));

/**
 * struct containing Tel62 board data
 */
struct TrbData {
//  uint32_t tdcWord :32;
	uint32_t time :19; //hit time measurement (100ps LSB)
	uint chID :5;      //TDC channel ID
	uint tdcID :4;     //TDC chip ID
	uint ID :4;        //0x4 (leading time), 0x5 (trailing time)
}__attribute__ ((__packed__));

class TrbDecoder {

public:
	TrbDecoder();
	virtual ~TrbDecoder();

	void GetData(uint, l0::MEPFragment*, uint32_t);

	/**
	 * Method returning the total number of edges found per Tel62 board
	 *
	 */
	uint GetNoEdgesPerTrb() {
		return nEdges_tot;
	}
	/**
	 * Method returning an array of edge times
	 *
	 */
	uint64_t* GetTimes() {
		return edge_times;
	}
	/**
	 * Method returning an array of edge channel IDs
	 *
	 */
	uint* GetChIDs() {
		return edge_chIDs;
	}
	/**
	 * Method returning an array of edge TDC IDs
	 *
	 */
	uint* GetTdcIDs() {
		return edge_tdcIDs;
	}
	/**
	 * Method returning an array of edge IDs (ID=4 for leading, ID=5 for trailing)
	 *
	 */
	uint* GetIDs() {
		return edge_IDs;
	}
	/**
	 * Method returning an array of edge Tel62 board ID
	 *
	 */
	uint* GetTrbIDs() {
		return edge_trbIDs;
	}

	/**
	 * Sets the number of FPGAs corresponding to the given FPGA flag. fpgaFlag must be element of [1,8]
	 * flag    	nFGPA
	 * 1		1
	 * 2		2
	 * 4		3
	 * 8		4
	 */
	static uint calculateNumberOfFPGAs(uint_fast8_t fpgaFlag) {
		return 32 - __builtin_clz(fpgaFlag);
	}

private:
	uint64_t frameTS;
	uint64_t time;
	uint nFPGAs;
	uint nFrames;
	uint nWordsPerFrame;
	uint nWords_tot;
	uint nEdges;
	uint nEdges_tot;	//total number of edges per Tel62 board
	TrbDataHeader* boardHeader;
	FPGADataHeader* fpgaHeader;
	FrameDataHeader* frameHeader;
	TrbData* tdcData;

	/**
	 * Arrays with edge info
	 *
	 */
	uint64_t* edge_times;
	uint* edge_chIDs;
	uint* edge_tdcIDs;
	uint* edge_IDs;
	uint* edge_trbIDs;

};

}

#endif /* TRBDATA_H_ */
