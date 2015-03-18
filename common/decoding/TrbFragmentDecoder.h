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

namespace na62 {

// forward declarations
namespace l0 {
class MEPFragment;
}

/**
 * struct containing Tel62 board header
 */
struct TrbDataHeader {
	uint8_t fpgaFlags :8;
	uint8_t triggerType :8;
	uint8_t sourceSubID :8; //Tel62 readout board ID
	uint8_t format :8;

	/**
	 * Sets the number of FPGAs corresponding to the given FPGA flag. fpgaFlag must be element of [1,8]
	 * flag    	nFGPA
	 * 1		1
	 * 2		2
	 * 4		3
	 * 8		4
	 */
	uint getNumberOfFPGAs() const {
		return 32 - __builtin_clz(fpgaFlags);
	}
}__attribute__ ((__packed__));

/**
 * struct containing FPGA header
 */
struct FPGADataHeader {
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
	uint16_t nWords :16;
}__attribute__ ((__packed__));

/**
 * struct containing Tel62 board data
 */
struct TrbData {
//  uint32_t tdcWord :32;
	uint time :19; //hit time measurement (100ps LSB)
	uint chID :5;      //TDC channel ID
	uint tdcID :4;     //TDC chip ID
	uint ID :4;        //0x4 (leading time), 0x5 (trailing time)
}__attribute__ ((__packed__));

class TrbFragmentDecoder {
	friend class DecoderHandler; // Only Decoder may access readData and isReady

	/**
	 * Returns true if readData has already been called an the getter functions are ready to be called
	 */
	bool isReady() const {
		return edgeTimes != nullptr;
	}

	/**
	 * Reads the raw data and fills the edge arrays
	 */
	void readData(const uint_fast16_t,
			const l0::MEPFragment* const trbDataFragment,
			const uint_fast32_t timestamp);

public:
	TrbFragmentDecoder();
	virtual ~TrbFragmentDecoder();

	/**
	 * Method returning the total number of edges found per Tel62 board
	 *
	 */
	inline uint getNumberOfEdgesPerTrb() const {
		return nEdges_tot;
	}

	/**
	 * Method returning an array of edge times
	 *
	 */
	inline const uint64_t* getTimes() const {
		return edgeTimes;
	}

	/**
	 * Method returning an array of edge channel IDs
	 *
	 */
	inline const uint_fast8_t* getChIDs() const {
		return edgeChIDs;
	}

	/**
	 * Method returning an array of edge TDC IDs
	 *
	 */
	inline const uint_fast8_t* getTdcIDs() const {
		return edgeTdcIDs;
	}

	/**
	 * Method returning an array of edge IDs (ID=4 for leading, ID=5 for trailing)
	 *
	 */
	inline const uint_fast8_t* getIDs() const {
		return edgeIDs;
	}

	/**
	 * Returns the subdetector specific ID of the fragment decoded by this object
	 */
	inline const uint_fast16_t getFragmentNumber() const {
		return fragmentNumber_;
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

	/**
	 * Arrays with edge info
	 *
	 */
	uint64_t* edgeTimes;
	uint_fast8_t * edgeChIDs;
	uint_fast8_t* edgeTdcIDs;
	uint_fast8_t* edgeIDs;

	uint_fast16_t fragmentNumber_;
};

}

#endif /* TRBDATA_H_ */
