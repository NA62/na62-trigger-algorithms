/*
 * L1Fragment.h
 *
 *  Created on: Jun 16, 2015
 *      Author: Angela Romano (angela.romano@cern.ch)
 */

#pragma once
#ifndef L1Fragment_H_
#define L1Fragment_H_

namespace na62 {

struct L1Global {
	uint32_t PCfarmSoftwareID;
	uint8_t refFineTime;
	uint8_t l0TriggerType;
	uint16_t l0TriggerFlags;
	uint8_t l1BypassProbability;
	uint8_t l1FlagMode :4;
	uint8_t refTimeSourceID :4; //not used yet
	uint16_t l1AutoFlagFactor;
	uint16_t l1ReductionFactor;
	uint16_t l1DownscaleFactor;
};

struct L1Algo{
	uint8_t l1AlgoID :4;
	uint8_t l1AlgoProcessID :4;
	uint8_t l1AlgoProcessed :2;
	uint8_t reserved;
	uint16_t l1AlgoDSFactor;
	uint8_t l1AlgoOnlineTimeWindow;
	uint8_t more_reserved;
};

struct L1Mask {
	uint8_t triggerWord;
	uint8_t numberOfEnabledAlgos :4;
	uint8_t numberOfFlaggedAlgos :4;
	uint8_t l1ReferenceFineTime;
	uint8_t l1ReferenceTimeSourceID :4;
	uint16_t reductionFactor;
	uint8_t algoEnableMask;
	uint8_t algoLogicMask;
	uint8_t algoFlagMask;
	uint8_t algoDwScMask;
	L1Algo l1Algo[4];
	uint32_t data;
};

struct L1Block {
	L1Global l1Global;
	L1Mask l1Mask[16];
};

}



#endif    /*  L1Fragment_H */
