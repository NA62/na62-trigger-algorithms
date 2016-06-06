/*
 * L2Fragment.h
 *
 *  Created on: Jun 16, 2015
 *      Author: Angela Romano (angela.romano@cern.ch)
 */

#pragma once
#ifndef L2Fragment_H_
#define L2Fragment_H_

namespace na62 {

struct L2Global {
	uint8_t l2BypassProbability;
	uint8_t l2FlagMode :4;
	uint8_t refTimeSourceID :4; //not used yet
	uint16_t l2AutoFlagFactor;
	uint16_t l2ReductionFactor;
	uint16_t l2DownscaleFactor;
};

struct L2Algo{
	uint8_t l2AlgoID :4;
	uint8_t l2AlgoProcessID :4;
	uint8_t l2AlgoProcessed :2;
	uint8_t reserved;
	uint16_t l2AlgoDSFactor;
	uint8_t l2AlgoOnlineTimeWindow;
	uint8_t more_reserved;
};

struct L2Mask {
	uint8_t triggerWord;
	uint8_t numberOfEnabledAlgos :4;
	uint8_t numberOfFlaggedAlgos :4;
	uint8_t l2ReferenceFineTime;
	uint8_t l2ReferenceTimeSourceID :4;
	uint16_t reductionFactor;
	uint8_t algoEnableMask;
	uint8_t algoLogicMask;
	uint8_t algoFlagMask;
	uint8_t algoDwScMask;
	L2Algo l2Algo[4];
	uint8_t maskID  :4;
	uint32_t data  : 28;
};

struct L2Block {
	L2Global l2Global;
	L2Mask l2Mask[16];
};

struct L2GLOBAL {
	uint32_t x;
	uint32_t y;
	uint32_t z;
};

struct L2MASK {
	uint32_t xx;
	uint32_t yy;
	uint32_t zz;
};

struct L2_BLOCK {
	uint32_t triggerword;
//	uint16_t globaldownscaling;
//	uint16_t globalreduction;
};

}


#endif    /*  L2Fragment_H */
