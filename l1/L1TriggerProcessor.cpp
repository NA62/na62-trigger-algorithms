/*
 * L1TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L1TriggerProcessor.h"
#include <options/Logging.h>
#include <bitset>

#include <eventBuilding/Event.h>
#include <structs/Event.h>
#include <eventBuilding/SourceIDManager.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>

#include "../common/decoding/DecoderHandler.h"
#include "../options/TriggerOptions.h"

#include "L1Fragment.h"
#include "KtagAlgo.h"
#include "CHODAlgo.h"
#include "RICHAlgo.h"
#include "LAVAlgo.h"
#include "IRC_SACAlgo.h"
#include "MUVAlgo.h"
#include "StrawAlgo.h"
#include "NewCHODAlgo.h"

#include <l1/ConfPath.h>

namespace na62 {
std::atomic<uint64_t>* L1TriggerProcessor::L1InputReducedEventsPerL0Mask_ = new std::atomic<uint64_t>[16];
std::atomic<uint64_t>** L1TriggerProcessor::EventCountersByL0MaskByAlgoID_;
std::atomic<uint64_t> L1TriggerProcessor::L1InputEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1InputReducedEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1AcceptedEvents_(0);

double L1TriggerProcessor::BypassProbability_ = 0.0;
uint L1TriggerProcessor::ReductionFactor_ = 0;
uint L1TriggerProcessor::DownscaleFactor_ = 0;
bool L1TriggerProcessor::FlagMode_ = 0;
uint L1TriggerProcessor::AutoFlagFactor_ = 0;
uint L1TriggerProcessor::ReferenceTimeSourceID_ = 0;

uint L1TriggerProcessor::NumberOfEnabledAlgos_[16];
uint L1TriggerProcessor::NumberOfFlaggedAlgos_[16];
uint L1TriggerProcessor::NumberOfEnabledAndFlaggedAlgos_[16];
uint L1TriggerProcessor::MaskReductionFactor_[16];
bool L1TriggerProcessor::MaskTimeoutFlag_[16];
uint_fast16_t L1TriggerProcessor::AlgoEnableMask_[16];
uint_fast16_t L1TriggerProcessor::AlgoEnableCutMask_[16];
uint_fast16_t L1TriggerProcessor::AlgoFlagMask_[16];
uint_fast16_t L1TriggerProcessor::AlgoLogicMask_[16];
uint_fast16_t L1TriggerProcessor::AlgoDwScMask_[16];
uint16_t L1TriggerProcessor::AlgoDwScFactor_[16][10];
int L1TriggerProcessor::AlgoProcessID_[16][10];

uint_fast16_t L1TriggerProcessor::ChodEnableMask_ = 0;
uint_fast16_t L1TriggerProcessor::RichEnableMask_ = 0;
uint_fast16_t L1TriggerProcessor::CedarEnableMask_ = 0;
uint_fast16_t L1TriggerProcessor::LavEnableMask_ = 0;
uint_fast16_t L1TriggerProcessor::StrawEnableMask_ = 0;
uint_fast16_t L1TriggerProcessor::IrcSacEnableMask_ = 0;
uint_fast16_t L1TriggerProcessor::MuvEnableMask_ = 0;
uint_fast16_t L1TriggerProcessor::NewChodEnableMask_ = 0;

uint_fast16_t L1TriggerProcessor::ChodFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::RichFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::CedarFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::LavFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::StrawFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::IrcSacFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::MuvFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::NewChodFlagMask_ = 0;

int L1TriggerProcessor::ChodAlgorithmId_;
int L1TriggerProcessor::RichAlgorithmId_;
int L1TriggerProcessor::CedarAlgorithmId_;
int L1TriggerProcessor::LavAlgorithmId_;
int L1TriggerProcessor::StrawAlgorithmId_;
int L1TriggerProcessor::IrcSacAlgorithmId_;
int L1TriggerProcessor::MuvAlgorithmId_;
int L1TriggerProcessor::NewChodAlgorithmId_;

int L1TriggerProcessor::StrawAlgoType_[16];
int L1TriggerProcessor::MUVAlgoType_[16];

uint L1TriggerProcessor::MaskIDToNum_[16];
int L1TriggerProcessor::NumToMaskID_[16];
uint L1TriggerProcessor::AlgoIDToNum_[16][10];
int L1TriggerProcessor::NumToAlgoID_[16][10];

uint_fast8_t L1TriggerProcessor::NumberOfEnabledL0Masks_ = 0;
std::vector<int> L1TriggerProcessor::L0MaskIDs_;
uint_fast32_t L1TriggerProcessor::L1DataPacketSize_ = 0;

/////////////////////
//Intended for the shared memory farm version do not use them and the related get methods
L1InfoToStorage L1TriggerProcessor::l1Info_;
bool L1TriggerProcessor::isL1WhileTimeout_;
///////////////////////

bool L1TriggerProcessor::isRequestZeroSuppressedCreamData(uint_fast8_t l1TriggerTypeWord) {
	// add here any special L1 trigger word requiring NZS LKr data
	return l1TriggerTypeWord != TRIGGER_L1_SPECIAL;
}

void L1TriggerProcessor::initialize(l1Struct &l1Struct) {
	// Seed for rand()
	srand(time(NULL));

	L0MaskIDs_ = TriggerOptions::GetIntList(OPTION_ACTIVE_L0_MASKS);
	NumberOfEnabledL0Masks_ = L0MaskIDs_.size();

	if (NumberOfEnabledL0Masks_ == 1 && L0MaskIDs_[0] == 99) { //Default value
		NumberOfEnabledL0Masks_ = 0;
		LOG_INFO("No Mask provided from the run control");
	}

//	LOG_INFO("numberOfEnabledL0Masks " << (uint)numberOfEnabledL0Masks);

	EventCountersByL0MaskByAlgoID_ = new std::atomic<uint64_t>*[16];

	for (int i = 0; i != 16; ++i) {
		EventCountersByL0MaskByAlgoID_[i] = new std::atomic<uint64_t>[10] { };
		L1InputReducedEventsPerL0Mask_[i] = 0;
		NumberOfEnabledAlgos_[i] = 0;
		NumberOfFlaggedAlgos_[i] = 0;
		NumberOfEnabledAndFlaggedAlgos_[i] = 0;
		MaskReductionFactor_[i] = 0;
		MaskTimeoutFlag_[i] = 0;
		AlgoEnableMask_[i] = 0;
		AlgoEnableCutMask_[i] = 0;
		AlgoFlagMask_[i] = 0;
		AlgoLogicMask_[i] = 0;
		AlgoDwScMask_[i] = 0;
		NumToMaskID_[i] = -1;
		MaskIDToNum_[i] = -1;
		StrawAlgoType_[i] = -1;
		MUVAlgoType_[i] = -1;
		for (int j = 0; j != 10; ++j) {
			EventCountersByL0MaskByAlgoID_[i][j] = 0;
			AlgoDwScFactor_[i][j] = 1;
			AlgoProcessID_[i][j] = -1;
			NumToAlgoID_[i][j] = -1;
			AlgoIDToNum_[i][j] = -1;
		}
	}

	BypassProbability_ = l1Struct.l1Global.l1BypassProbability;
	ReductionFactor_ = l1Struct.l1Global.l1ReductionFactor;
	DownscaleFactor_ = l1Struct.l1Global.l1DownscaleFactor;
	FlagMode_ = (bool) l1Struct.l1Global.l1FlagMode;
	AutoFlagFactor_ = l1Struct.l1Global.l1AutoFlagFactor;
	ReferenceTimeSourceID_ = l1Struct.l1Global.l1ReferenceTimeSourceID;

	L1Downscaling::initialize();
	L1Reduction::initialize();

	LAVAlgo::loadConfigurationFile(LAV_CONFIG_FILE);

	for (int i = 0; i != 16; i++) {
		/*
		 * Initialisation of 16 L0 trigger masks
		 */
		if (!i) {
			ChodAlgorithmId_ = l1Struct.l1Mask[i].chod.configParams.l1TrigMaskID;
			RichAlgorithmId_ = l1Struct.l1Mask[i].rich.configParams.l1TrigMaskID;
			CedarAlgorithmId_ = l1Struct.l1Mask[i].ktag.configParams.l1TrigMaskID;
			LavAlgorithmId_ = l1Struct.l1Mask[i].lav.configParams.l1TrigMaskID;
			StrawAlgorithmId_ = l1Struct.l1Mask[i].straw.configParams.l1TrigMaskID;
			IrcSacAlgorithmId_ = l1Struct.l1Mask[i].ircsac.configParams.l1TrigMaskID;
			MuvAlgorithmId_ = l1Struct.l1Mask[i].muv.configParams.l1TrigMaskID;
			NewChodAlgorithmId_ = l1Struct.l1Mask[i].newchod.configParams.l1TrigMaskID;
		} else {
			if ((ChodAlgorithmId_ != l1Struct.l1Mask[i].chod.configParams.l1TrigMaskID)
					|| (RichAlgorithmId_ != l1Struct.l1Mask[i].rich.configParams.l1TrigMaskID)
					|| (CedarAlgorithmId_ != l1Struct.l1Mask[i].ktag.configParams.l1TrigMaskID)
					|| (LavAlgorithmId_ != l1Struct.l1Mask[i].lav.configParams.l1TrigMaskID)
					|| (StrawAlgorithmId_ != l1Struct.l1Mask[i].straw.configParams.l1TrigMaskID)
					|| (IrcSacAlgorithmId_ != l1Struct.l1Mask[i].ircsac.configParams.l1TrigMaskID)
					|| (MuvAlgorithmId_ != l1Struct.l1Mask[i].muv.configParams.l1TrigMaskID)
					|| (NewChodAlgorithmId_ != l1Struct.l1Mask[i].newchod.configParams.l1TrigMaskID))
				LOG_ERROR("Mismatch between AlgoID !!!");
			// Throw Exception!
		}
		NumberOfEnabledAlgos_[i] = l1Struct.l1Mask[i].numberOfEnabledAlgos;
		NumberOfFlaggedAlgos_[i] = l1Struct.l1Mask[i].numberOfFlaggedAlgos;
		MaskReductionFactor_[i] = l1Struct.l1Mask[i].maskReductionFactor;

		AlgoEnableMask_[i] = (l1Struct.l1Mask[i].newchod.configParams.l1TrigEnable << NewChodAlgorithmId_)
				| (l1Struct.l1Mask[i].muv.configParams.l1TrigEnable << MuvAlgorithmId_)
				| (l1Struct.l1Mask[i].straw.configParams.l1TrigEnable << StrawAlgorithmId_)
				| (l1Struct.l1Mask[i].ircsac.configParams.l1TrigEnable << IrcSacAlgorithmId_)
				| (l1Struct.l1Mask[i].lav.configParams.l1TrigEnable << LavAlgorithmId_)
				| (l1Struct.l1Mask[i].ktag.configParams.l1TrigEnable << CedarAlgorithmId_)
				| (l1Struct.l1Mask[i].chod.configParams.l1TrigEnable << ChodAlgorithmId_);
		if (NumberOfEnabledAlgos_[i] != __builtin_popcount((uint) AlgoEnableMask_[i])) {
			LOG_ERROR("Mismatch between NumberOfEnabledAlgos and algoEnableMask !!!");
			NumberOfEnabledAlgos_[i] = __builtin_popcount((uint) AlgoEnableMask_[i]);
		}

		AlgoFlagMask_[i] = (l1Struct.l1Mask[i].newchod.configParams.l1TrigFlag << NewChodAlgorithmId_)
				| (l1Struct.l1Mask[i].muv.configParams.l1TrigFlag << MuvAlgorithmId_)
				| (l1Struct.l1Mask[i].straw.configParams.l1TrigFlag << StrawAlgorithmId_)
				| (l1Struct.l1Mask[i].ircsac.configParams.l1TrigFlag << IrcSacAlgorithmId_)
				| (l1Struct.l1Mask[i].lav.configParams.l1TrigFlag << LavAlgorithmId_)
				| (l1Struct.l1Mask[i].ktag.configParams.l1TrigFlag << CedarAlgorithmId_)
				| (l1Struct.l1Mask[i].chod.configParams.l1TrigFlag << ChodAlgorithmId_);

//		std::bitset<16> enableMask(AlgoEnableMask_[i]);
//		std::bitset<16> flagMask(AlgoFlagMask_[i]);
//		std::bitset<16> enableFlagMask(AlgoEnableMask_[i] & AlgoFlagMask_[i]);
//		std::bitset<16> enableCutMask(AlgoEnableMask_[i] & (0xFF - AlgoFlagMask_[i]));
//		LOG_INFO("enableMask " << enableMask);
//		LOG_INFO("flagMask " << flagMask);
//		LOG_INFO("enableFlagMask " << enableFlagMask);
//		LOG_INFO("enableCutMask " << enableCutMask);

		AlgoEnableCutMask_[i] = AlgoEnableMask_[i] & (0xFF - AlgoFlagMask_[i]);
		NumberOfEnabledAndFlaggedAlgos_[i] = __builtin_popcount((uint) (AlgoEnableMask_[i] & AlgoFlagMask_[i]));
//		LOG_INFO("L0 Mask " << i << " number of Enabled algo " << NumberOfEnabledAlgos_[i]);
//		LOG_INFO(" number of Flagged algo " << NumberOfFlaggedAlgos_[i] << " EnabledAndFlagged " << NumberOfEnabledAndFlaggedAlgos_[i]);
//		LOG_INFO(" number of Enabled algo " << NumberOfEnabledAlgos_[i] << " EnabledAndCut " << __builtin_popcount((uint) AlgoEnableCutMask_[i]));

		if (NumberOfFlaggedAlgos_[i] != NumberOfEnabledAndFlaggedAlgos_[i]) {
//			LOG_ERROR("This is a warning: Mismatch between NumberOfFlaggedAlgos and algoEnabledAndFlaggedMask");
			NumberOfFlaggedAlgos_[i] = NumberOfEnabledAndFlaggedAlgos_[i];
		}

		AlgoLogicMask_[i] = (l1Struct.l1Mask[i].newchod.configParams.l1TrigLogic << NewChodAlgorithmId_)
				| (l1Struct.l1Mask[i].muv.configParams.l1TrigLogic << MuvAlgorithmId_)
				| (l1Struct.l1Mask[i].straw.configParams.l1TrigLogic << StrawAlgorithmId_)
				| (l1Struct.l1Mask[i].ircsac.configParams.l1TrigLogic << IrcSacAlgorithmId_)
				| (l1Struct.l1Mask[i].lav.configParams.l1TrigLogic << LavAlgorithmId_)
				| (l1Struct.l1Mask[i].ktag.configParams.l1TrigLogic << CedarAlgorithmId_)
				| (l1Struct.l1Mask[i].chod.configParams.l1TrigLogic << ChodAlgorithmId_);

		AlgoDwScMask_[i] = (l1Struct.l1Mask[i].newchod.configParams.l1TrigDownScale << NewChodAlgorithmId_)
				| (l1Struct.l1Mask[i].muv.configParams.l1TrigDownScale << MuvAlgorithmId_)
				| (l1Struct.l1Mask[i].straw.configParams.l1TrigDownScale << StrawAlgorithmId_)
				| (l1Struct.l1Mask[i].ircsac.configParams.l1TrigDownScale << IrcSacAlgorithmId_)
				| (l1Struct.l1Mask[i].lav.configParams.l1TrigDownScale << LavAlgorithmId_)
				| (l1Struct.l1Mask[i].ktag.configParams.l1TrigDownScale << CedarAlgorithmId_)
				| (l1Struct.l1Mask[i].chod.configParams.l1TrigDownScale << ChodAlgorithmId_);

		AlgoDwScFactor_[i][(uint) ChodAlgorithmId_] = l1Struct.l1Mask[i].chod.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) RichAlgorithmId_] = l1Struct.l1Mask[i].rich.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) CedarAlgorithmId_] = l1Struct.l1Mask[i].ktag.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) LavAlgorithmId_] = l1Struct.l1Mask[i].lav.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) StrawAlgorithmId_] = l1Struct.l1Mask[i].straw.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) IrcSacAlgorithmId_] = l1Struct.l1Mask[i].ircsac.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) MuvAlgorithmId_] = l1Struct.l1Mask[i].muv.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) NewChodAlgorithmId_] = l1Struct.l1Mask[i].newchod.configParams.l1TrigDSFactor;

		AlgoProcessID_[i][(uint) ChodAlgorithmId_] = l1Struct.l1Mask[i].chod.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) RichAlgorithmId_] = l1Struct.l1Mask[i].rich.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) CedarAlgorithmId_] = l1Struct.l1Mask[i].ktag.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) LavAlgorithmId_] = l1Struct.l1Mask[i].lav.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) StrawAlgorithmId_] = l1Struct.l1Mask[i].straw.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) IrcSacAlgorithmId_] = l1Struct.l1Mask[i].ircsac.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) MuvAlgorithmId_] = l1Struct.l1Mask[i].muv.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) NewChodAlgorithmId_] = l1Struct.l1Mask[i].newchod.configParams.l1TrigProcessID;

		CHODAlgo::initialize(i, l1Struct.l1Mask[i].chod);
//		RICHAlgo::initialize(l1Struct.l1Mask[i].rich);
		KtagAlgo::initialize(i, l1Struct.l1Mask[i].ktag);
		LAVAlgo::initialize(i, l1Struct.l1Mask[i].lav);
		StrawAlgo::initialize(i, l1Struct.l1Mask[i].straw);
		IRC_SACAlgo::initialize(i, l1Struct.l1Mask[i].ircsac);
		MUV3Algo::initialize(i, l1Struct.l1Mask[i].muv);
		NewCHODAlgo::initialize(i, l1Struct.l1Mask[i].newchod);

		ChodEnableMask_ |= (l1Struct.l1Mask[i].chod.configParams.l1TrigEnable << i);
		RichEnableMask_ |= (l1Struct.l1Mask[i].rich.configParams.l1TrigEnable << i);
		CedarEnableMask_ |= (l1Struct.l1Mask[i].ktag.configParams.l1TrigEnable << i);
		LavEnableMask_ |= (l1Struct.l1Mask[i].lav.configParams.l1TrigEnable << i);
		StrawEnableMask_ |= (l1Struct.l1Mask[i].straw.configParams.l1TrigEnable << i);
		IrcSacEnableMask_ |= (l1Struct.l1Mask[i].ircsac.configParams.l1TrigEnable << i);
		MuvEnableMask_ |= (l1Struct.l1Mask[i].muv.configParams.l1TrigEnable << i);
		NewChodEnableMask_ |= (l1Struct.l1Mask[i].newchod.configParams.l1TrigEnable << i);

		ChodFlagMask_ |= (l1Struct.l1Mask[i].chod.configParams.l1TrigFlag << i);
		RichFlagMask_ |= (l1Struct.l1Mask[i].rich.configParams.l1TrigFlag << i);
		CedarFlagMask_ |= (l1Struct.l1Mask[i].ktag.configParams.l1TrigFlag << i);
		LavFlagMask_ |= (l1Struct.l1Mask[i].lav.configParams.l1TrigFlag << i);
		StrawFlagMask_ |= (l1Struct.l1Mask[i].straw.configParams.l1TrigFlag << i);
		IrcSacFlagMask_ |= (l1Struct.l1Mask[i].ircsac.configParams.l1TrigFlag << i);
		MuvFlagMask_ |= (l1Struct.l1Mask[i].muv.configParams.l1TrigFlag << i);
		NewChodFlagMask_ |= (l1Struct.l1Mask[i].newchod.configParams.l1TrigFlag << i);

		StrawAlgoType_[i] = l1Struct.l1Mask[i].straw.xx;
		MUVAlgoType_[i] = l1Struct.l1Mask[i].muv.xx;
//		LOG_INFO("L0Mask " << i << " StrawAlgoType " << StrawAlgoType_[i]);
//		LOG_INFO("L0Mask " << i << " MUVAlgoType " << MUVAlgoType_[i]);
	}



	L1DataPacketSize_ = sizeof(L1Global);

	int num = 0;
	int algoNum = 0;
	int algoSumProcessIds = 0;
	int algoTotSumProcessIds = 0;
	if(NumberOfEnabledL0Masks_) { // fix for L1 data packet when physics triggers are disabled
		for (int l0Mask : L0MaskIDs_) {
			NumToMaskID_[num] = l0Mask;
			MaskIDToNum_[l0Mask] = num;
//			LOG_INFO("Initialization of Enabled Masks " << MaskIDToNum_[l0Mask] << " " << NumToMaskID_[num]);
//			LOG_INFO("is Straw enabled on mask " << l0Mask << " ? " << (uint)(StrawEnableMask_ & (1 << l0Mask)));
			if (!(StrawEnableMask_ & (1 << l0Mask)))
				L1DataPacketSize_ += (sizeof(L1Mask) + NumberOfEnabledAlgos_[l0Mask] * sizeof(L1Algo));
			else
				L1DataPacketSize_ += (sizeof(L1Mask) + (NumberOfEnabledAlgos_[l0Mask] - 1) * sizeof(L1Algo)) + sizeof(L1StrawAlgo);
			for (int i = 0; i != 16; i++) {
				if (AlgoEnableMask_[l0Mask] & (1 << i)) {
					NumToAlgoID_[num][algoNum] = i;
					AlgoIDToNum_[num][i] = algoNum;
//					LOG_INFO("Initialization of Enabled Algos " << AlgoIDToNum_[num][i] << " " << NumToAlgoID_[num][algoNum]);
					algoSumProcessIds += AlgoProcessID_[l0Mask][NumToAlgoID_[num][algoNum]];
					algoNum++;
				}
			}
			algoTotSumProcessIds = NumberOfEnabledAlgos_[l0Mask]*(NumberOfEnabledAlgos_[l0Mask]-1)/2.;
//			LOG_INFO("Algo Tot Sum " << algoTotSumProcessIds << " Algo Sum " << algoSumProcessIds);
			if(algoSumProcessIds != algoTotSumProcessIds){
				LOG_ERROR("Configuration error in trigger mask " << l0Mask << ": check algo process ordering in Run Control!!!");
				MaskTimeoutFlag_[l0Mask] = true;
			}
			num++;
			algoNum = 0;
			algoSumProcessIds=0;
			algoTotSumProcessIds=0;
		}
	}
//	LOG_INFO("L1 Global " << sizeof(L1Global) << " L1Mask " << sizeof(L1Mask));
//	LOG_INFO("L1Algo " << sizeof(L1Algo) << " L1StrawAlgo " << sizeof(L1StrawAlgo));
//	LOG_INFO("L1 Data Packet Size " << (uint)L1DataPacketSize_);
}

uint_fast8_t L1TriggerProcessor::compute(Event* const event, StrawAlgo& strawalgo) {

	uint_fast8_t l1Trigger = 0;
	uint_fast8_t l1GlobalFlagTrigger = 0;
	uint_fast8_t l1MaskFlagTrigger = 0;
	bool isL1Bypassed = 0;
	bool isAllL1AlgoDisable = 0;
	bool isL1WhileTimeout = 0;
	uint_fast8_t numberOfTriggeredL1Masks = 0;
	bool isL0PhysicsTrigger = 0;
//	bool isL0PeriodicTrigger = 0;
//	bool isL0ControlTrigger = 0;
	bool isReducedEvent = 0;
//	bool isAlgoEnableForAllL0Masks = 0;
	uint_fast8_t chodTrigger = 0;
//	uint_fast8_t richTrigger = 0;
	uint_fast8_t cedarTrigger = 0;
	uint_fast8_t lavTrigger = 0;
	uint_fast8_t ircsacTrigger = 0;
	uint_fast8_t strawTriggerTmp = 0;
	uint_fast8_t strawTrigger = 0;
	uint_fast8_t muvTrigger = 0;
	uint_fast8_t newchodTrigger = 0;

	L1InfoToStorage l1Info;

	DecoderHandler decoder(event);


	L1InputEvents_.fetch_add(1, std::memory_order_relaxed);

	/*
	 * Check if the event is autopass
	 */
	if (FlagMode_) {
		l1GlobalFlagTrigger = 1;
	} else if (AutoFlagFactor_ && (L1InputEvents_ % AutoFlagFactor_ == 0)) {
		l1GlobalFlagTrigger = 1;
	}

	/*
	 * Check if the event should bypass the processing (Special, Periodics and Control)
	 */
	if (event->isSpecialTriggerEvent()) {
		isL1Bypassed = 1;
		event->setRrequestZeroSuppressedCreamData(false);
		l1Trigger = ((uint) l1GlobalFlagTrigger << 7) | ((l1MaskFlagTrigger != 0) << 6) | (isL1Bypassed << 5) | (isAllL1AlgoDisable << 4)
				| (numberOfTriggeredL1Masks != 0);
		L1TriggerProcessor::writeL1Data(event, &l1Info);
//		L1TriggerProcessor::readL1Data(event);
		return l1Trigger;
	}

	/*
	 * Check if the event (PERIODICS INCLUDED) fulfills the reduction option
	 *
	 */
	if (ReductionFactor_ && (L1InputEvents_ % ReductionFactor_ != 0))
		return 0;

	L1InputReducedEvents_.fetch_add(1, std::memory_order_relaxed);

	if ((event->isPeriodicTriggerEvent() && event->isPulserGTKTriggerEvent()) || bypassEvent()) {
		isL1Bypassed = 1;
		event->setRrequestZeroSuppressedCreamData(true);
		l1Trigger = ((uint) l1GlobalFlagTrigger << 7) | ((l1MaskFlagTrigger != 0) << 6) | (isL1Bypassed << 5) | (isAllL1AlgoDisable << 4)
				| (numberOfTriggeredL1Masks != 0);
		L1TriggerProcessor::writeL1Data(event, &l1Info);
//		L1TriggerProcessor::readL1Data(event);
		return l1Trigger;
	}

	/*
	 * Check if the event fulfills the reduction option
	 * TODO::Add possibility to reduce periodics or not
	 *
	 */
//	if (ReductionFactor_ && (L1InputEvents_ % ReductionFactor_ != 0))
//		return 0;
//	L1InputReducedEvents_.fetch_add(1, std::memory_order_relaxed);

	/*
	 * The event is ready to be processed
	 *
	 */
	uint_fast16_t l0TrigFlags = event->getTriggerFlags();
	uint_fast8_t refFineTime = event->getFinetime();
	l1Info.setL1RefTimeL0TP(refFineTime);

	/*
	 * Store the global event timestamp taken from the reference detector
	 */
	l0::MEPFragment* tsFragment = event->getL0SubeventBySourceIDNum(SourceIDManager::TS_SOURCEID_NUM)->getFragment(0);
	event->setTimestamp(tsFragment->getTimestamp());

	uint_fast8_t l1TriggerTmp;
	uint_fast8_t l1TriggerResult;
	uint_fast8_t l1FlagTrigger;
	uint watchingWhileLoops = 0;

	if (event->isPhysicsTriggerEvent()) {
		isL0PhysicsTrigger = 1;
	}
	if (event->isPeriodicTriggerEvent()) {
//		isL0PeriodicTrigger = 1;
		isL1Bypassed = 1;
	}
	if (event->isControlTriggerEvent()) {
//		isL0ControlTrigger = 1;
		isL1Bypassed = 1;
	}
	if (isL0PhysicsTrigger) {
		for (int i = 0; i != 16; i++) {
			l1TriggerResult = 0;
			if (l0TrigFlags & (1 << i)) {
				L1InputReducedEventsPerL0Mask_[i].fetch_add(1, std::memory_order_relaxed);
				int_fast32_t l1ProcessID = 0;
				watchingWhileLoops = 0;
				l1TriggerTmp = 0;
				l1FlagTrigger = 0;

				if (NumberOfEnabledAndFlaggedAlgos_[i])
					l1FlagTrigger = 1;

//				if set here would flag all events triggered by the mask with the algo enabled in flagging mode
//				while it should flag only events being processed by the specific algo enabled in flagging mode
//				l1MaskFlagTrigger += l1FlagTrigger;

				if (MaskReductionFactor_[i] && (L1InputReducedEventsPerL0Mask_[i] % MaskReductionFactor_[i] != 0))
					isReducedEvent = 1;
				else
					isReducedEvent = 0;

				if (!NumberOfEnabledAlgos_[i])
					isAllL1AlgoDisable = 1;

				if(MaskTimeoutFlag_[i])
					isL1WhileTimeout = true;

				while ((watchingWhileLoops != 10) && !isReducedEvent && l1ProcessID != NumberOfEnabledAlgos_[i]) {

					if ((ChodEnableMask_ & (1 << i)) && (AlgoProcessID_[i][ChodAlgorithmId_] == l1ProcessID)
							&& SourceIDManager::isChodActive()) {
						if (!l1Info.isL1CHODProcessed()) {
							chodTrigger = CHODAlgo::processCHODTrigger(i, decoder, &l1Info);
//							if (chodTrigger != 0) {
//								L1Downscaling::processAlgorithm (chodAlgorithmId);
//							}
						}
						l1ProcessID++;
//						printf("L1TriggerProcessor.cpp: chodTrigger %d\n", chodTrigger);

						if (AlgoLogicMask_[i] & (1 << (uint) ChodAlgorithmId_)) {
							l1TriggerTmp |= (chodTrigger << (uint) ChodAlgorithmId_);
							l1Info.setL1CHODTrgWrd(i,chodTrigger);
						} else {
							l1TriggerTmp |= (not chodTrigger << (uint) ChodAlgorithmId_);
							l1Info.setL1CHODTrgWrd(i,not chodTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
//					if ((ChodEnableMask_ & l0TrigFlags) == l0TrigFlags) isAlgoEnableForAllL0Masks = 1;
//					if (!(l1TriggerTmp & AlgoEnableMask_[i]) && l1ProcessID && !((ChodEnableMask_ & ChodFlagMask_) & (1 << i))) {
					if (((AlgoEnableMask_[i] & (1 << ChodAlgorithmId_)) && !(l1TriggerTmp & (1 << ChodAlgorithmId_))) && l1ProcessID
							&& !((ChodEnableMask_ & ChodFlagMask_) & (1 << i))) {
						watchingWhileLoops++;
						break;
					}

					if ((CedarEnableMask_ & (1 << i)) && AlgoProcessID_[i][CedarAlgorithmId_] == l1ProcessID
							&& SourceIDManager::isCedarActive()) {
						if (!l1Info.isL1KTAGProcessed()) {
							cedarTrigger = KtagAlgo::processKtagTrigger(i, decoder, &l1Info);
						}
						l1ProcessID++;
//						printf("L1TriggerProcessor.cpp: cedarTrigger %d\n", cedarTrigger);

						if (AlgoLogicMask_[i] & (1 << (uint) CedarAlgorithmId_)) {
							l1TriggerTmp |= (cedarTrigger << (uint) CedarAlgorithmId_);
							l1Info.setL1KTAGTrgWrd(i,cedarTrigger);
						} else {
							l1TriggerTmp |= (not cedarTrigger << (uint) CedarAlgorithmId_);
							l1Info.setL1KTAGTrgWrd(i,not cedarTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
//					if ((CedarEnableMask_ & l0TrigFlags) == l0TrigFlags) isAlgoEnableForAllL0Masks = 1;
//					if (!(l1TriggerTmp & AlgoEnableMask_[i]) && l1ProcessID && !((CedarEnableMask_ & CedarFlagMask_) & (1 << i))) {
					if (((AlgoEnableMask_[i] & (1 << CedarAlgorithmId_)) && !(l1TriggerTmp & (1 << CedarAlgorithmId_))) && l1ProcessID
							&& !((CedarEnableMask_ & CedarFlagMask_) & (1 << i))) {
						watchingWhileLoops++;
						break;
					}

					if ((LavEnableMask_ & (1 << i)) && AlgoProcessID_[i][LavAlgorithmId_] == l1ProcessID
							&& SourceIDManager::isLavActive()) {
						if (!l1Info.isL1LAVProcessed()) {
							lavTrigger = LAVAlgo::processLAVTrigger(i, decoder, &l1Info);
						}
						l1ProcessID++;
//						printf("L1TriggerProcessor.cpp: lavTrigger %d\n", lavTrigger);

						if (AlgoLogicMask_[i] & (1 << (uint) LavAlgorithmId_)) {
							l1TriggerTmp |= (lavTrigger << (uint) LavAlgorithmId_);
							l1Info.setL1LAVTrgWrd(i,lavTrigger);
						} else {
							l1TriggerTmp |= (not lavTrigger << (uint) LavAlgorithmId_);
							l1Info.setL1LAVTrgWrd(i,not lavTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
//					if ((LavEnableMask_ & l0TrigFlags) == l0TrigFlags) isAlgoEnableForAllL0Masks = 1;
//					if (!(l1TriggerTmp & AlgoEnableMask_[i]) && l1ProcessID && !((LavEnableMask_ & LavFlagMask_) & (1 << i))) {
					if (((AlgoEnableMask_[i] & (1 << LavAlgorithmId_)) && !(l1TriggerTmp & (1 << LavAlgorithmId_))) && l1ProcessID
							&& !((LavEnableMask_ & LavFlagMask_) & (1 << i))) {
						watchingWhileLoops++;
						break;
					}

					if ((IrcSacEnableMask_ & (1 << i)) && AlgoProcessID_[i][IrcSacAlgorithmId_] == l1ProcessID
							&& SourceIDManager::isIrcActive()) {
						if (!l1Info.isL1IRCSACProcessed()) {
							ircsacTrigger = IRC_SACAlgo::processIRCSACTrigger(i, decoder, &l1Info);
						}
						l1ProcessID++;
//						printf("L1TriggerProcessor.cpp: ircsacTrigger %d\n", ircsacTrigger);

						if (AlgoLogicMask_[i] & (1 << (uint) IrcSacAlgorithmId_)) {
							l1TriggerTmp |= (ircsacTrigger << (uint) IrcSacAlgorithmId_);
							l1Info.setL1IRCSACTrgWrd(i,ircsacTrigger);
						} else {
							l1TriggerTmp |= (not ircsacTrigger << (uint) IrcSacAlgorithmId_);
							l1Info.setL1IRCSACTrgWrd(i,not ircsacTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
//					if ((IrcSacEnableMask_ & l0TrigFlags) == l0TrigFlags) isAlgoEnableForAllL0Masks = 1;
//					if (!(l1TriggerTmp & AlgoEnableMask_[i]) && l1ProcessID && !((IrcSacEnableMask_ & IrcSacFlagMask_) & (1 << i))) {
					if (((AlgoEnableMask_[i] & (1 << IrcSacAlgorithmId_)) && !(l1TriggerTmp & (1 << IrcSacAlgorithmId_))) && l1ProcessID
							&& !((IrcSacEnableMask_ & IrcSacFlagMask_) & (1 << i))) {
						watchingWhileLoops++;
						break;
					}

					if ((StrawEnableMask_ & (1 << i)) && AlgoProcessID_[i][StrawAlgorithmId_] == l1ProcessID
							&& SourceIDManager::isStrawActive()) {
						if (!l1Info.isL1StrawProcessed()) {
							strawTriggerTmp = strawalgo.processStrawTrigger(i, decoder, &l1Info);
						}
						l1ProcessID++;
//						printf("L1TriggerProcessor.cpp: strawTrigger %d\n", strawTrigger);

						/*
						 * StrawAlgoType_[i]: 0 for pnn, 1 for exotics
						 */
						if (!StrawAlgoType_[i])
							strawTrigger = (strawTriggerTmp & 0x1);
						else if (StrawAlgoType_[i] == 1)
							strawTrigger = (strawTriggerTmp & 0x2) >> 1;

						if (AlgoLogicMask_[i] & (1 << (uint) StrawAlgorithmId_)) {
							l1TriggerTmp |= (strawTrigger << (uint) StrawAlgorithmId_);
							l1Info.setL1StrawTrgWrd(i,strawTrigger);
						} else {
							l1TriggerTmp |= (not strawTrigger << (uint) StrawAlgorithmId_);
							l1Info.setL1StrawTrgWrd(i,not strawTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
//					if ((StrawEnableMask_ & l0TrigFlags) == l0TrigFlags) isAlgoEnableForAllL0Masks = 1;
//					if (!(l1TriggerTmp & AlgoEnableMask_[i]) && l1ProcessID && !((StrawEnableMask_ & StrawFlagMask_) & (1 << i))) {
					if (((AlgoEnableMask_[i] & (1 << StrawAlgorithmId_)) && !(l1TriggerTmp & (1 << StrawAlgorithmId_))) && l1ProcessID
							&& !((StrawEnableMask_ & StrawFlagMask_) & (1 << i))) {
						watchingWhileLoops++;
						break;
					}

					if ((MuvEnableMask_ & (1 << i)) && AlgoProcessID_[i][MuvAlgorithmId_] == l1ProcessID
							&& SourceIDManager::isMUV3Active()) {
						/*
						 * AlgoType=0, DRY RUN 2017, Multiplicity trigger (TIGHT HIT) to be used in positive, corresponding to Trigger3
						 * AlgoType=0, Multiplicity trigger (LOOSE HIT) to be used in veto, corresponding to Trigger3
						 * AlgoType=1, Left-Right trigger to be used in positive, corresponding to Trigger1
						 * AlgoType=2, Neighbours trigger (from Chris) to be used in positive, corresponding to Trigger0
						 */
						if (!MUVAlgoType_[i] && !l1Info.isL1MUV3TriggerMultiProcessed()) {
							muvTrigger = MUV3Algo::processMUV3Trigger3(i, decoder, &l1Info);
						} else if ((MUVAlgoType_[i] == 1) && !l1Info.isL1MUV3TriggerLeftRightProcessed()) {
							muvTrigger = MUV3Algo::processMUV3Trigger1(i, decoder, &l1Info);
						} else if ((MUVAlgoType_[i] == 2) && !l1Info.isL1MUV3TriggerNeighboursProcessed()) {
							muvTrigger = MUV3Algo::processMUV3Trigger0(i, decoder, &l1Info);
						}
						l1ProcessID++;
//						printf("L1TriggerProcessor.cpp: muvTrigger %d\n", muvTrigger);

						if (AlgoLogicMask_[i] & (1 << (uint) MuvAlgorithmId_)) {
							l1TriggerTmp |= (muvTrigger << (uint) MuvAlgorithmId_);
							l1Info.setL1MUV3TrgWrd(i,muvTrigger);
						} else {
							l1TriggerTmp |= (not muvTrigger << (uint) MuvAlgorithmId_);
							l1Info.setL1MUV3TrgWrd(i,not muvTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
//					if ((MuvEnableMask_ & l0TrigFlags) == l0TrigFlags) isAlgoEnableForAllL0Masks = 1;
//					if (!(l1TriggerTmp & AlgoEnableMask_[i]) && l1ProcessID && !((MuvEnableMask_ & MuvFlagMask_) & (1 << i))) {
					if (((AlgoEnableMask_[i] & (1 << MuvAlgorithmId_)) && !(l1TriggerTmp & (1 << MuvAlgorithmId_))) && l1ProcessID
							&& !((MuvEnableMask_ & MuvFlagMask_) & (1 << i))) {
						watchingWhileLoops++;
						break;
					}

					if ((NewChodEnableMask_ & (1 << i)) && AlgoProcessID_[i][NewChodAlgorithmId_] == l1ProcessID
							&& SourceIDManager::isIrcActive()) {
						if (!l1Info.isL1NewCHODProcessed()) {
							newchodTrigger = NewCHODAlgo::processNewCHODTrigger(i, decoder, &l1Info);
						}
						l1ProcessID++;
//						printf("L1TriggerProcessor.cpp: newchodTrigger %d\n", newchodTrigger);

						if (AlgoLogicMask_[i] & (1 << (uint) NewChodAlgorithmId_)) {
							l1TriggerTmp |= (newchodTrigger << (uint) NewChodAlgorithmId_);
							l1Info.setL1NewCHODTrgWrd(i,newchodTrigger);
						} else {
							l1TriggerTmp |= (not newchodTrigger << (uint) NewChodAlgorithmId_);
							l1Info.setL1NewCHODTrgWrd(i,not newchodTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
//					if ((NewChodEnableMask_ & l0TrigFlags) == l0TrigFlags) isAlgoEnableForAllL0Masks = 1;
//					if (!(l1TriggerTmp & AlgoEnableMask_[i]) && l1ProcessID && !((NewChodEnableMask_ & NewChodFlagMask_) & (1 << i))) {
					if (((AlgoEnableMask_[i] & (1 << NewChodAlgorithmId_)) && !(l1TriggerTmp & (1 << NewChodAlgorithmId_))) && l1ProcessID
							&& !((NewChodEnableMask_ & NewChodFlagMask_) & (1 << i))) {
						watchingWhileLoops++;
						break;
					}

					watchingWhileLoops++;
					if (!l1ProcessID && watchingWhileLoops)
						LOG_ERROR("No Algorithms are being processed within the first While Loop !");
				}

				if ((watchingWhileLoops == 10) && (l1ProcessID != NumberOfEnabledAlgos_[i])) {
					isL1WhileTimeout = true;
					LOG_ERROR("TimeOut while loop: check why enabled algos do not get processed !");
				}

				if (!isReducedEvent
						&& (!AlgoEnableMask_[i] || ((l1TriggerTmp & AlgoEnableMask_[i]) == AlgoEnableMask_[i])
								|| ((l1TriggerTmp & AlgoEnableCutMask_[i]) == AlgoEnableCutMask_[i]))) {

//					LOG_INFO("TrigTmp " << (uint) l1TriggerTmp << " dwscMask " << (uint) algoDwScMask[i]);
//					LOG_INFO("downscale Factor " << algoDwScFactor[i][__builtin_ctz( (uint) algoDwScMask[i])]);

					l1MaskFlagTrigger += l1FlagTrigger;

					if ((l1TriggerTmp & AlgoDwScMask_[i])
							&& (++(EventCountersByL0MaskByAlgoID_[i][__builtin_ctz((uint) AlgoDwScMask_[i])])
									% AlgoDwScFactor_[i][__builtin_ctz((uint) AlgoDwScMask_[i])] != 0)) {

//						isDownscaledAndFlaggedEvent += ((uint) l1FlagTrigger);
//						LOG_INFO("flagTrig " << (uint) l1FlagTrigger << " isDownscaledAndFlaggedEvent " << isDownscaledAndFlaggedEvent);
//						if (isDownscaledAndFlaggedEvent) {
//						LOG_INFO("flagTrig " << (uint) l1FlagTrigger << " isDownscaledAndFlaggedEvent " << isDownscaledAndFlaggedEvent);
//							l1TriggerWords[i] = ((l1TriggerTmp
//									& algoEnableMask[i]) == algoEnableMask[i]);
//							l1TriggerWords[i] = l1TriggerTmp;
//						} else
						l1TriggerResult = 0;
					} else {
						l1TriggerResult = ((l1TriggerTmp & AlgoEnableMask_[i]) == AlgoEnableMask_[i]);
					}
				} else {
					l1TriggerResult = 0;
				}
//				printf("L1TriggerProcessor.cpp: MaskId %d l1TriggerResult %x\n", i, l1TriggerResult);
				event->setL1TriggerWord(i, l1TriggerResult);
			}
			if (__builtin_popcount((uint) l1TriggerResult)) {
				numberOfTriggeredL1Masks++;
			}
		}
//		printf("Summary of Triggered Masks: %d\n", numberOfTriggeredL1Masks);
//		for (int i = 0; i != 16; i++) printf("Summary of Trigger Words: MaskId %d l1Trigger %x\n", i, event->getL1TriggerWord(i));
	}

	/*
	 * Final L1 trigger word calculation
	 */

	l1Trigger = ((uint) l1GlobalFlagTrigger << 7) | ((l1MaskFlagTrigger != 0) << 6) | (isL1Bypassed << 5) | (isAllL1AlgoDisable << 4)
			| (isL1WhileTimeout << 3) | (numberOfTriggeredL1Masks != 0);


	if (l1Trigger != 0) {
		if (l1Trigger & TRIGGER_L1_PHYSICS) {
			L1AcceptedEvents_.fetch_add(1, std::memory_order_relaxed);
		}

		/*
		 * Check if event is downscaled
		 */
		if (DownscaleFactor_ && (L1AcceptedEvents_ % DownscaleFactor_ != 0))
			return 0;

		/*
		 * Decision whether or not to request zero suppressed data from the creams
		 */
		event->setRrequestZeroSuppressedCreamData(true);
		event->setProcessingID(0); // 0 indicates raw data as collected from the detector

		/////////////////////
		//Intended for the shared memory farm version do not use them and the related get methods
		l1Info_ = l1Info;
		isL1WhileTimeout_ = isL1WhileTimeout;
		///////////////////////
		L1TriggerProcessor::writeL1Data(event, &l1Info, isL1WhileTimeout);
//		L1TriggerProcessor::readL1Data(event);
	}

//	std::bitset<8> l1TrgWrd(l1Trigger);
//	LOG_INFO("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger " << l1TrgWrd);
	return l1Trigger;
}

void L1TriggerProcessor::readL1Data(Event* const event) {
	const l0::MEPFragment* const L1TPEvent = event->getL1ResultSubevent()->getFragment(0);
	char* pload = (char*) L1TPEvent->getPayload();
	uint_fast16_t length = (uint_fast16_t) L1TPEvent->getPayloadLength();
	if ((uint) length != (uint) L1DataPacketSize_)
		LOG_ERROR("length (in bytes) " << (uint)length << " is different from l1DataPacketsSize " << (uint)L1DataPacketSize_);
	for (uint i = 0; i != (uint) L1DataPacketSize_ / 4; i++) {
		L1Data* dataPacket = (L1Data*) pload + i;
		std::bitset<32> data((uint) dataPacket->dataWords);
//		LOG_INFO("L1 Data " << data);
	}
}

void L1TriggerProcessor::writeL1Data(Event* const event, L1InfoToStorage* l1Info, bool isL1WhileTimeout) {

	uint nBlockHeaderWords = 0;
	uint_fast8_t refTimeSourceID = 0;
	uint_fast8_t refTimeSourceID_tmp = 0;

	const l0::MEPFragment* const L1TPEvent = event->getL1ResultSubevent()->getFragment(0);

	if (L1TPEvent->getPayloadLength() != L1DataPacketSize_) {
		LOG_ERROR(
				"L1 result fragment has wrong size " << (int) L1TPEvent->getPayloadLength()<< " instead of " << L1DataPacketSize_ << ". Result will not be filled.");
		return;
	}
	char* payload = const_cast<char*>(L1TPEvent->getPayload());

	L1Global* globalPacket = reinterpret_cast<L1Global*>(payload);

	globalPacket->globalPacketLength = sizeof(L1Global);
	globalPacket->refFineTime = event->getFinetime();
	globalPacket->refTimeSourceID = ReferenceTimeSourceID_;
	globalPacket->flagMode = FlagMode_;
	globalPacket->format = 1; //1 Starting from run 8520
	globalPacket->downscaleFactor = DownscaleFactor_;
	globalPacket->reductionFactor = ReductionFactor_;
	globalPacket->numberOfEnabledMasks = NumberOfEnabledL0Masks_;
	globalPacket->bypassProbability = BypassProbability_;
	globalPacket->autoFlagFactor = AutoFlagFactor_;

	nBlockHeaderWords += sizeof(L1Global); //3 32-bit header words for global part

//	LOG_INFO("Global Packet Length " << (uint)globalPacket->globalPacketLength << " refFineTime " << (uint)globalPacket->refFineTime);
//	LOG_INFO("RefTimeSourceID "<< (uint)globalPacket->refTimeSourceID << " flagMode " << (uint)globalPacket->flagMode);
//	LOG_INFO("Data Format " << (uint)globalPacket->format);
//	LOG_INFO("DownscaleFactor " << (uint)globalPacket->downscaleFactor << " reductionFactor "<< (uint)globalPacket->reductionFactor);
//	LOG_INFO("Number of Enabled Masks " << (uint)globalPacket->numberOfEnabledMasks);
//	LOG_INFO("BypassProb " << (uint)globalPacket->bypassProbability);
//	LOG_INFO("AutoFlagFactor " << (uint)globalPacket->autoFlagFactor);

	uint nMaskWords = 0;
	int numToMaskID;
	int numToAlgoID;
	bool isStrawAlgoEnabled;

	for (int iNum = 0; iNum < NumberOfEnabledL0Masks_; iNum++) {
		isStrawAlgoEnabled = false;

		if (NumToMaskID_[iNum] == -1)
			LOG_ERROR("ERROR! Wrong association of mask ID!");
		else
			numToMaskID = NumToMaskID_[iNum];

		if (StrawEnableMask_ & (1 << numToMaskID))
			isStrawAlgoEnabled = true;

		L1Mask* maskPacket = (L1Mask*) (payload + nBlockHeaderWords + nMaskWords);

//		LOG_INFO("NumToMaskID " << numToMaskID);
		maskPacket->numberOfEnabledAlgos = NumberOfEnabledAlgos_[numToMaskID];
		maskPacket->triggerWord = event->getL1TriggerWord(numToMaskID);
		maskPacket->maskID = numToMaskID;
		maskPacket->flags = 0;
		if (NumberOfEnabledAndFlaggedAlgos_[numToMaskID]) {
			maskPacket->flags |= (1 << 6);
		}
		if (!NumberOfEnabledAlgos_[numToMaskID]) {
			maskPacket->flags |= (1 << 4);
		}
		if (isL1WhileTimeout) {
			maskPacket->flags |= (1 << 2);
		}
		maskPacket->reductionFactor = MaskReductionFactor_[numToMaskID];
		maskPacket->reserved = 0;

		nMaskWords += sizeof(L1Mask); //2 32-bit header words for each mask

//		LOG_INFO("numberOfEnabledAlgos " << (uint)maskPacket->numberOfEnabledAlgos);
//		LOG_INFO("triggerWord " << (uint) maskPacket->triggerWord);
//		LOG_INFO("maskID " << (uint) maskPacket->maskID);

//		std::bitset<8> maskFlag((uint) maskPacket->flags);
//		LOG_INFO("Flags " << (uint)maskPacket->flags << " " << maskFlag);
//		LOG_INFO("reductionFactor " << (uint)maskPacket->reductionFactor);

		for (uint iAlgoNum = 0; iAlgoNum < NumberOfEnabledAlgos_[numToMaskID]; iAlgoNum++) {
			if (NumToAlgoID_[iNum][iAlgoNum] == -1)
				LOG_ERROR("ERROR! Wrong association of algo ID!");
			else
				numToAlgoID = NumToAlgoID_[iNum][iAlgoNum];

//			LOG_INFO("isStrawEnabled " << isStrawAlgoEnabled << " " << (numToAlgoID == StrawAlgorithmId_));
			if (isStrawAlgoEnabled && (numToAlgoID == StrawAlgorithmId_)) {

				L1StrawAlgo* strawAlgoPacket = (L1StrawAlgo*) (payload + nBlockHeaderWords + nMaskWords);

				if (((uint) event->getTriggerFlags()) & (1 << numToMaskID)) {

//					LOG_INFO("NumToAlgoID " << numToAlgoID);
					strawAlgoPacket->processID = AlgoProcessID_[numToMaskID][numToAlgoID];
					strawAlgoPacket->algoID = numToAlgoID;
					strawAlgoPacket->downscaleFactor = AlgoDwScFactor_[numToMaskID][numToAlgoID];

//					LOG_INFO("Enable " << ((AlgoEnableMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
//					LOG_INFO("Flag " << ((AlgoFlagMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
//					LOG_INFO("Logic " << ((AlgoLogicMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
//					LOG_INFO("DwSc " << ((AlgoDwScMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));

					strawAlgoPacket->algoFlags = (((AlgoEnableMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID) << 6)
							| (((AlgoFlagMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID) << 4)
							| (((AlgoLogicMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID) << 2)
							| (((AlgoDwScMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));

					L1TriggerProcessor::writeStrawAlgoPacket(numToAlgoID, strawAlgoPacket, numToMaskID, l1Info);

					refTimeSourceID_tmp = (strawAlgoPacket->qualityFlags & 0x80); // due to change in data format
					if (refTimeSourceID != refTimeSourceID_tmp)
						refTimeSourceID = refTimeSourceID_tmp;
				} else {
					std::memset(strawAlgoPacket, 0, sizeof(L1StrawAlgo));
				}

				strawAlgoPacket->numberOfWords = (sizeof(L1StrawAlgo) / 4.);
				nMaskWords += sizeof(L1StrawAlgo);

//				LOG_INFO("Number of Words " << (uint) strawAlgoPacket->numberOfWords);
//				LOG_INFO("Algo ID " << (uint) strawAlgoPacket->algoID << " Algo ProcessID " << (uint)strawAlgoPacket->processID);
//				LOG_INFO("Quality Flags " << (uint) strawAlgoPacket->qualityFlags << " Algo Flags " << (uint)strawAlgoPacket->algoFlags);

//				std::bitset<8> algoFlag((uint) strawAlgoPacket->algoFlags);
//				std::bitset<8> qualityFlag((uint) strawAlgoPacket->qualityFlags);
//				LOG_INFO("Quality Flags " << qualityFlag << " Algo Flags " << algoFlag);

//				LOG_INFO("DS Factor " << (uint)strawAlgoPacket->downscaleFactor);
//				LOG_INFO("Online TW " << (uint)strawAlgoPacket->onlineTimeWindow);
//				LOG_INFO("L1 Data[0] " << (uint) strawAlgoPacket->l1Data[0] << " L1 Data[1] " << (uint)strawAlgoPacket->l1Data[1]);

			} else {
				L1Algo* algoPacket = (L1Algo*) (payload + nBlockHeaderWords + nMaskWords);

				if (((uint) event->getTriggerFlags()) & (1 << numToMaskID)) {

//					LOG_INFO("NumToAlgoID " << numToAlgoID);
					algoPacket->processID = AlgoProcessID_[numToMaskID][numToAlgoID];
					algoPacket->algoID = numToAlgoID;
					algoPacket->downscaleFactor = AlgoDwScFactor_[numToMaskID][numToAlgoID];

//					LOG_INFO("Enable " << ((AlgoEnableMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
//					LOG_INFO("Flag " << ((AlgoFlagMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
//					LOG_INFO("Logic " << ((AlgoLogicMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
//					LOG_INFO("DwSc " << ((AlgoDwScMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));

					algoPacket->algoFlags = (((AlgoEnableMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID) << 6)
							| (((AlgoFlagMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID) << 4)
							| (((AlgoLogicMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID) << 2)
							| (((AlgoDwScMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));

					L1TriggerProcessor::writeAlgoPacket(numToAlgoID, algoPacket, numToMaskID, l1Info);

					refTimeSourceID_tmp = (algoPacket->qualityFlags & 0x80); // due to change in data format
					if (refTimeSourceID != refTimeSourceID_tmp)
						refTimeSourceID = refTimeSourceID_tmp;
				} else {
					std::memset(algoPacket, 0, sizeof(L1Algo));
				}

				algoPacket->numberOfWords = (sizeof(L1Algo) / 4.);
				nMaskWords += sizeof(L1Algo);

//				LOG_INFO("Number of Words " << (uint) algoPacket->numberOfWords);
//				LOG_INFO("Algo ID " << (uint) algoPacket->algoID << " Algo ProcessID " << (uint)algoPacket->processID);
//				LOG_INFO("Quality Flags " << (uint) algoPacket->qualityFlags << " Algo Flags " << (uint)algoPacket->algoFlags);

//				std::bitset<8> algoFlag((uint) algoPacket->algoFlags);
//				std::bitset<8> qualityFlag((uint) algoPacket->qualityFlags);
//				LOG_INFO("Quality Flags " << qualityFlag << " Algo Flags " << algoFlag);

//				LOG_INFO("DS Factor " << (uint)algoPacket->downscaleFactor);
//				LOG_INFO("Online TW " << (uint)algoPacket->onlineTimeWindow);
//				LOG_INFO("L1 Data[0] " << (uint) algoPacket->l1Data[0] << " L1 Data[1] " << (uint)algoPacket->l1Data[1]);
			}
		}

		maskPacket->referenceTimeSourceID = refTimeSourceID;
		if (!refTimeSourceID)
			maskPacket->referenceFineTime = event->getFinetime();
		else if (refTimeSourceID == 1 && l1Info->isL1CHODProcessed())
			maskPacket->referenceFineTime = (uint8_t) l1Info->getCHODAverageTime();
//		LOG_INFO("Mask Ref Finetime " << (uint)maskPacket->referenceFineTime);
//		LOG_INFO("Mask Ref Finetime Source ID " << (uint)maskPacket->referenceTimeSourceID);
	}

}

bool L1TriggerProcessor::writeAlgoPacket(int algoID, L1Algo* algoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {
	switch (algoID) {
	case 0:
		CHODAlgo::writeData(algoPacket, l0MaskID, l1Info);
		return true;
	case 1:
//		RICHAlgo::writeData(algoPacket,l0MaskID);
		return true;
	case 2:
		KtagAlgo::writeData(algoPacket, l0MaskID, l1Info);
		return true;
	case 3:
		LAVAlgo::writeData(algoPacket, l0MaskID, l1Info);
		return true;
	case 4:
		IRC_SACAlgo::writeData(algoPacket, l0MaskID, l1Info);
		return true;
//	case 5:
//		StrawAlgo::writeData(algoPacket, l0MaskID, l1Info);
//		return true;
	case 6:
		MUV3Algo::writeData(algoPacket, l0MaskID, l1Info);
		return true;
	case 7:
		NewCHODAlgo::writeData(algoPacket, l0MaskID, l1Info);
		return true;
	default:
		return false;
	}
}

bool L1TriggerProcessor::writeStrawAlgoPacket(int algoID, L1StrawAlgo* strawAlgoPacket, uint l0MaskID, L1InfoToStorage* l1Info) {
	switch (algoID) {
	case 5:
		StrawAlgo::writeData(strawAlgoPacket, l0MaskID, l1Info);
		return true;
	default:
		return false;
	}
}

std::string L1TriggerProcessor::algoIdToTriggerName(uint algoID) {
	switch (algoID) {
	case 0:
		return "CHOD";
	case 1:
		return "RICH";
	case 2:
		return "KTAG";
	case 3:
		return "LAV";
	case 4:
		return "IRCSAC";
	case 5:
		return "STRAW";
	case 6:
		return "MUV3";
	case 7:
		return "NEWCHOD";
	default:
		return "UNKNOWN ALGO ID!";
	}
}

}
/* namespace na62 */
