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

//namespace na62 {
//class l0::MEPFragment;
//} /* namespace na62 */

namespace na62 {

struct TrbDataHeader //Board header
{
	uint8_t flags :8;
	uint8_t triggerType :8;
	uint8_t sourceSubID :8; //Tel Readout Board ID
	uint8_t format :8;
}__attribute__ ((__packed__));

struct FPGADataHeader //FPGA header
{
	uint8_t noFrame :8;
	uint8_t noNonEmptyFrame :8;
	uint8_t FPGAID :8;
	uint8_t errFlags :8;
}__attribute__ ((__packed__));

struct FrameDataHeader //Frame header
{
	uint16_t coarseFrameTime :16;
	uint16_t nWordsPerFrame :16;
}__attribute__ ((__packed__));

struct TrbData {
	//uint32_t tdcWord :32;
	uint32_t Time :19; //hit time measurement (100ps LSB)
	uint8_t chID :5;   //TDC channel ID
	uint8_t tdcID :4;  //TDC chip ID
	uint8_t ID :4;     //0x4 (leading time), 0x5 (trailing time)
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
	//uint8_t* noFrame;                   //array maxNFPGA size
	//uint8_t* noNonEmptyFrame;           //array maxNFPGA size
	//uint8_t* FPGAID;                    //array maxNFPGA size
	//uint8_t* errFlags;                  //array maxNFPGA size
	//uint16_t** coarseFrameTime;         //array maxNFrame size
	//uint16_t** nWordsPerFrame;          //array maxNFrame size

	//uint8_t* GetNoFrame();
	//uint8_t* GetNoNonEmptyFrame();
	//uint8_t* GetFPGAID();
	//uint8_t* GetErrFlags();
	//uint16_t** GetCoarseFrameTime();
	//uint16_t** GetNWordsPerFrame();

private:

};

}

#endif /* CEDARALGORITM_H_ */
