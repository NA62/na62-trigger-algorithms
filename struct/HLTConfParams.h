/*
 * L1ConfParams.h
 *
 *  Created on: 19 Apr 2016
 *      Author: romano
 */

#ifndef STRUCT_HLTCONFPARAMS_H_
#define STRUCT_HLTCONFPARAMS_H_

typedef struct l1MaskSubStruct_t { //Configuration parameters depending on L0 mask
	int l1TrigProcessID; //Identify the time order of the L1 algo processing
	int l1TrigMaskID; //0 for CHOD, 1 for RICH, 2 for KTAG, 3 for LAV, 4 for MUV3, 5 for Straw
	int l1TrigEnable;
	int l1TrigLogic;
	int l1TrigFlag;
	int l1TrigDownScale;
	int l1TrigDSFactor;
	int l1TrigRefTimeSourceID;
	double l1TrigOnlineTimeWindow;
} l1MaskSubStruct;

typedef struct l2MaskSubStruct_t { //Configuration parameters depending on L0 mask
	int l2TrigProcessID; //Identify the time order of the L2 algo processing
	int l2TrigID; //0 for LKr, 1 for MUV1, 2 for MUV2, 3 for GTK...
	int l2TrigEnable;
	int l2TrigLogic;
	int l2TrigFlag;
	int l2TrigDownScale;
	int l2TrigDSFactor;
	int l2TrigRefTimeSourceID;
	double l2TrigOnlineTimeWindow;
} l2MaskSubStruct;

typedef struct l1CHOD_t {
	l1MaskSubStruct configParams;
	int xx;
} l1CHOD;
typedef struct l1RICH_t {
	l1MaskSubStruct configParams;
	int xx;
} l1RICH;
typedef struct l1KTAG_t {
	l1MaskSubStruct configParams;
	int xx;
} l1KTAG;
typedef struct l1LAV_t {
	l1MaskSubStruct configParams;
	int xx;
} l1LAV;
typedef struct l1IRCSAC_t {
	l1MaskSubStruct configParams;
	int xx;
} l1IRCSAC;
typedef struct l1Straw_t {
	l1MaskSubStruct configParams;
	int xx;
} l1Straw;
typedef struct l1MUV_t {
	l1MaskSubStruct configParams;
	int xx;
} l1MUV;
typedef struct l1NewCHOD_t {
	l1MaskSubStruct configParams;
	int xx;
} l1NewCHOD;

typedef struct l2LKr_t {
	l2MaskSubStruct configParams;
	int xx;
} l2LKr;

typedef struct l1MaskStruct_t {
	int numberOfEnabledAlgos;
	int numberOfFlaggedAlgos;
	int maskReductionFactor;
	l1KTAG ktag;
	l1CHOD chod;
	l1RICH rich;
	l1LAV lav;
	l1IRCSAC ircsac;
	l1Straw straw;
	l1MUV muv;
	l1NewCHOD newchod;
	///....all L1 Algorithms
} l1MaskStruct;

typedef struct l2MaskStruct_t {
	l2LKr lkr;
	///....all L2 algorithms
} l2MaskStruct;

typedef struct l1GlobalStruct_t {
	double l1BypassProbability;
	int l1ReductionFactor;
	int l1DownscaleFactor;
	int l1FlagMode;
	int l1AutoFlagFactor;
	int l1ReferenceTimeSourceID;
} l1GlobalStruct;

typedef struct l2GlobalStruct_t {
	double l2BypassProbability;
	int l2ReductionFactor;
	int l2DownscaleFactor;
	int l2FlagMode;
	int l2AutoFlagFactor;
	int l2ReferenceTimeSourceID;
} l2GlobalStruct;

typedef struct l1Struct_t {
	l1GlobalStruct l1Global;
	l1MaskStruct l1Mask[16]; //for 16 L0 masks
} l1Struct;

typedef struct l2Struct_t {
	l2GlobalStruct l2Global;
	l2MaskStruct l2Mask[16]; //for 16 L0 masks
} l2Struct;

typedef struct HLTStruct_t {
	l1Struct l1;
	l2Struct l2;
} HLTStruct;

#endif /* STRUCT_HLTCONFPARAMS_H_ */
