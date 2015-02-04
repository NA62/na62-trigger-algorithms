/*
 * CedarData.h
 *
 *  Created on: Nov 24, 2014
 *      Author: angela romano
 *      Email: axr@hep.ph.bham.ac.uk
 */

#ifndef CEDARALGORITM_H_
#define CEDARALGORITM_H_

#include <sys/types.h>
#include <cstdint>
#include <l0/MEPFragment.h>

namespace na62 {

struct TrbDataHeader //Tel62 readout board header
{
	uint8_t flags :8;
	uint8_t triggerType :8;
	uint8_t sourceSubID :8; //Tel62 readout board ID
	uint8_t format :8;
}__attribute__ ((__packed__));

struct FPGADataHeader //FPGA header
{
	uint noFrame :8;
	uint noNonEmptyFrame :8;
	uint FPGAID :8;
	uint errFlags :8;
}__attribute__ ((__packed__));

struct FrameDataHeader //Frame header
{
	uint16_t coarseFrameTime :16;
	uint nWordsPerFrame :16;
}__attribute__ ((__packed__));

struct TrbData {
//  uint32_t tdcWord :32;
	uint32_t Time :19; //hit time measurement (100ps LSB)
	uint chID :5;   //TDC channel ID
	uint tdcID :4;  //TDC chip ID
	uint ID :4;     //0x4 (leading time), 0x5 (trailing time)
}__attribute__ ((__packed__));

class CedarData {

public:
	CedarData();
	virtual ~CedarData();
	void SetHits(l0::MEPFragment*);

	uint nhits;
	uint nhits_tot;
	uint nWords;
	TrbDataHeader* cedarHeader;           //array maxNTEL size
	FPGADataHeader** cedar_fpgaHeader;    //array maxNFPGA size
	FrameDataHeader*** cedar_frameHeader; //array maxNFrame size
	TrbData** tdc_data;

	uint* noFrame;                   //array maxNFPGA size
	uint* noNonEmptyFrame;           //array maxNFPGA size
	uint* FPGAID;                    //array maxNFPGA size
	uint* errFlags;                  //array maxNFPGA size
	uint16_t** coarseFrameTime;      //array maxNFrame size
	uint** nWordsPerFrame;       //array maxNFrame size

	uint32_t* time;
	uint* chID;
	uint* tdcID;
	uint* ID;

	uint* GetNoFrame() {
		return noFrame;
	}
	;
	void SetNoFrame(uint* buffer) {
		noFrame = buffer;
	}
	;
	uint* GetNoNonEmptyFrame() {
		return noNonEmptyFrame;
	}
	;
	void SetNoNonEmptyFrame(uint* buffer) {
		noNonEmptyFrame = buffer;
	}
	;

	uint* GetFPGAID() {
		return FPGAID;
	}
	;
	void SetFPGAID(uint* buffer) {
		FPGAID = buffer;
	}
	;

	uint* GetErrFlags() {
		return errFlags;
	}
	;
	void SetErrFlags(uint* buffer) {
		errFlags = buffer;
	}
	;
	uint16_t** GetCoarseFrameTime() {
		return coarseFrameTime;
	}
	;
	void SetCoarseFrameTime(uint16_t** buffer) {
		coarseFrameTime = buffer;
	}
	;

	uint** GetNWordsPerFrame() {
		return nWordsPerFrame;
	}
	;
	void SetNWordsPerFrame(uint** buffer) {
		nWordsPerFrame = buffer;
	}
	;

	uint32_t* GetTime() {
		return time;
	}
	;
	void SetTime(uint32_t* buffer) {
		time = buffer;
	}
	;
	uint* GetChID() {
		return chID;
	}
	;
	void SetChID(uint* buffer) {
		chID = buffer;
	}
	;
	uint* GetTdcID() {
		return tdcID;
	}
	;
	void SetTdcID(uint* buffer) {
		tdcID = buffer;
	}
	;
	uint* GetID() {
		return ID;
	}
	;
	void SetID(uint* buffer) {
		ID = buffer;
	}
	;

private:

};

}

#endif /* CEDARALGORITM_H_ */
