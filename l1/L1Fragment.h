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
//	uint32_t PCfarmSoftwareID;
	uint8_t globalPacketLength; //length of global packet in bytes
	uint8_t refFineTime;
	uint8_t refTimeSourceID :4; //not used yet
	uint8_t flagMode :4;
	uint8_t format;
	uint16_t downscaleFactor;
	uint16_t reductionFactor;
	uint8_t numberOfEnabledMasks;
	uint8_t bypassProbability;
	uint16_t autoFlagFactor;
};

struct L1Algo {
	uint8_t numberOfWords; //number of 32bit words in Algo packet, 2 header words are included
	uint8_t processID;
	uint8_t algoID;
	uint8_t qualityFlags; //isEmptyPacket, isBadData, isProcessed
	uint16_t downscaleFactor;
	uint8_t onlineTimeWindow; //half width of online matching time window
//	uint8_t l1AlgoProcessed :2;
	uint8_t algoFlags; //enable, logic, flagging, downscale
//	uint8_t more_reserved;
	uint32_t l1Data[2];
};

struct L1StrawAlgo {
	uint8_t numberOfWords; //number of 32bit words in Algo packet, 2 header words are included
	uint8_t processID;
	uint8_t algoID;
	uint8_t qualityFlags; //isEmptyPacket, isBadData, isProcessed
	uint16_t downscaleFactor;
	uint8_t onlineTimeWindow; //half width of online matching time window
//	uint8_t l1AlgoProcessed :2;
	uint8_t algoFlags; //enable, logic, flagging, downscale
//	uint8_t more_reserved;
	uint32_t l1Data[6];
};

struct L1Data {
	uint32_t dataWords;
};

struct L1Mask {
	uint8_t numberOfEnabledAlgos;
	uint8_t triggerWord;
	uint8_t maskID;
	uint8_t flags;
	uint16_t reductionFactor;
	uint8_t referenceFineTime;
	uint8_t referenceTimeSourceID :4;
	uint8_t reserved :4;
};

struct L1Block {
	L1Global l1Global;
	L1Mask l1Mask[16];
	L1Algo l1Algo[16][16];
};

struct L1GLOBAL {
	uint32_t x;
	uint32_t y;
	uint32_t z;
};

struct L1MASK {
	uint32_t xx;
	uint32_t yy;
	uint32_t zz;
};

struct L1_BLOCK {
	uint32_t triggerword;
//	uint16_t globaldownscaling;
//	uint16_t globalreduction;
};

}

#endif    /*  L1Fragment_H */
