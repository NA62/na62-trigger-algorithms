/*
 * SrbDecoder.h
 *
 */

#ifndef SRBDECODER_H_
#define SRBDECODER_H_

#include <sys/types.h>
#include <cstdint>
#include <boost/noncopyable.hpp>

namespace na62 {

// forward declarations
namespace l0 {
class Subevent;
class MEPFragment;
}

/**
 * struct containing SRB board header
 */
struct SrbDataHeader {
	uint8_t srbID :5;
	uint8_t flags :3;
	uint8_t triggerType :8;
	uint16_t packetLength :16;
	uint32_t firstTSCoarseTime :32;
}__attribute__ ((__packed__));

/**
 * struct containing SRB board data
 */
struct SrbData {
//  uint32_t srbWord :32;
	uint fineTime :5;      //hit time (LSB = 780 ps)
	uint edgeType :1;      //edge type (0=leading, 1=trailing)
	uint strawID :8;       //identifier of the individual straw
	uint errorFlag :2;
}__attribute__ ((__packed__));

struct SrbTimeSlot {
	uint SlotCounter :8;
}__attribute__ ((__packed__));

class SrbFragmentDecoder: private boost::noncopyable {
	friend class DecoderHandler; // Only Decoder may access readData and isReady

public:
	SrbFragmentDecoder();
	virtual ~SrbFragmentDecoder();

	/**
	 * Returns true if readData has already been called an the getter functions are ready to be called
	 */
	bool isReady() const {
		return edgeTimes != nullptr;
	}

	/**
	 * Reads the raw data and fills the edge arrays
	 */
	void readData(uint_fast32_t timestamp);

	/**
	 * Method returning the total number of edges found per Srb
	 *
	 */
	inline uint getNumberOfEdgesStored() const {
		return nEdges_tot;
	}

	/**
	 * Method returning an array of edge times
	 *
	 */
	inline const double* getTimes() const {
		return edgeTimes;
	}

	/**
	 * Method returning an array of edge Straw IDs
	 *
	 */
	inline const uint_fast8_t* getStrawIDs() const {
		return edgeStrawIDs;
	}

	/**
	 * Method returning Srb ID for current fragment
	 *
	 */
	inline const uint_fast8_t* getSrbIDs() const {
		return edgeSrbIDs;
	}

	/**
	 * Returns an array of booleans for every edge declaring whether the edge is a leading (true) or trailing (false) edge
	 *
	 */
	inline const bool* getIsLeadings() const {
		return edgeIsLeading;
	}

	/**
	 * Returns the Srb specific ID of the fragment decoded by this object
	 */
	inline const uint_fast16_t getFragmentNumber() const {
		return fragmentNumber_;
	}

	void setDataSource(const l0::Subevent* subevent, uint_fast16_t fragmentNumber) {
		subevent_ = subevent;
		fragmentNumber_ = fragmentNumber;
	}
	bool isBadFragment() {
		return isBadFrag_;
	}

private:
	int64_t firstTSCoarseTime;
	uint nEdges_tot;	//total number of edges per SRB board
	bool isBadFrag_;

	/**
	 * Arrays with edge info
	 *
	 */

	//check data types for the following
	double* edgeTimes;
	uint_fast8_t* edgeStrawIDs;
	uint_fast8_t* edgeSrbIDs;
	uint_fast8_t * edgeErrorFlags;
	bool* edgeIsLeading;

	const l0::Subevent* subevent_;
	uint_fast16_t fragmentNumber_;
};

}

#endif /* SRBDATA_H_ */
