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
#include "NewCHODAlgo.h"

namespace na62 {

std::atomic<uint64_t>* L1TriggerProcessor::L1Triggers_ = new std::atomic<uint64_t>[0xFF + 1];
std::atomic<uint64_t>* L1TriggerProcessor::L1AcceptedEventsPerL0Mask_ = new std::atomic<uint64_t>[16];
std::atomic<uint64_t>* L1TriggerProcessor::L1InputReducedEventsPerL0Mask_ = new std::atomic<uint64_t>[16];
std::atomic<uint64_t>** L1TriggerProcessor::EventCountersByL0MaskByAlgoID_;
std::atomic<uint64_t> L1TriggerProcessor::L1InputEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1InputReducedEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1InputEventsPerBurst_(0);
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
uint_fast16_t L1TriggerProcessor::IrcSacEnableMask_ = 0;
uint_fast16_t L1TriggerProcessor::MuvEnableMask_ = 0;
uint_fast16_t L1TriggerProcessor::NewChodEnableMask_ = 0;

uint_fast16_t L1TriggerProcessor::ChodFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::RichFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::CedarFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::LavFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::IrcSacFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::MuvFlagMask_ = 0;
uint_fast16_t L1TriggerProcessor::NewChodFlagMask_ = 0;

uint L1TriggerProcessor::ChodAlgorithmId_;
uint L1TriggerProcessor::RichAlgorithmId_;
uint L1TriggerProcessor::CedarAlgorithmId_;
uint L1TriggerProcessor::LavAlgorithmId_;
uint L1TriggerProcessor::IrcSacAlgorithmId_;
uint L1TriggerProcessor::MuvAlgorithmId_;
uint L1TriggerProcessor::NewChodAlgorithmId_;

uint L1TriggerProcessor::MaskIDToNum_[16];
uint L1TriggerProcessor::NumToMaskID_[16];
uint L1TriggerProcessor::AlgoIDToNum_[16][10];
uint L1TriggerProcessor::NumToAlgoID_[16][10];

uint_fast8_t L1TriggerProcessor::NumberOfEnabledL0Masks_ = 0;
std::vector<int> L1TriggerProcessor::L0MaskIDs_;
uint_fast32_t L1TriggerProcessor::L1DataPacketSize_ = 0;

bool L1TriggerProcessor::isRequestZeroSuppressedCreamData(uint_fast8_t l1TriggerTypeWord) {
	// add here any special L1 trigger word requiring NZS LKr data
	return l1TriggerTypeWord != TRIGGER_L1_SPECIAL;
}

void L1TriggerProcessor::initialize(l1Struct &l1Struct) {
	// Seed for rand()
	srand(time(NULL));

	L0MaskIDs_ = TriggerOptions::GetIntList(OPTION_ACTIVE_L0_MASKS);
	NumberOfEnabledL0Masks_ = L0MaskIDs_.size();
//	LOG_INFO("numberOfEnabledL0Masks " << (uint)numberOfEnabledL0Masks);

	for (int i = 0; i != 0xFF + 1; i++) {
		L1Triggers_[i] = 0;
	}
	EventCountersByL0MaskByAlgoID_ = new std::atomic<uint64_t>*[16];

	for (int i = 0; i != 16; ++i) {
		EventCountersByL0MaskByAlgoID_[i] = new std::atomic<uint64_t>[10] { };
		L1AcceptedEventsPerL0Mask_[i] = 0;
		L1InputReducedEventsPerL0Mask_[i] = 0;
		NumberOfEnabledAlgos_[i] = 0;
		NumberOfFlaggedAlgos_[i] = 0;
		NumberOfEnabledAndFlaggedAlgos_[i] = 0;
		MaskReductionFactor_[i] = 0;
		AlgoEnableMask_[i] = 0;
		AlgoEnableCutMask_[i] = 0;
		AlgoFlagMask_[i] = 0;
		AlgoLogicMask_[i] = 0;
		AlgoDwScMask_[i] = 0;
		NumToMaskID_[i] = -1;
		MaskIDToNum_[i] = -1;
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

	for (int i = 0; i != 16; i++) {
		/*
		 * Initialisation of 16 L0 trigger masks
		 */
		if (!i) {
			ChodAlgorithmId_ = l1Struct.l1Mask[i].chod.configParams.l1TrigMaskID;
			RichAlgorithmId_ = l1Struct.l1Mask[i].rich.configParams.l1TrigMaskID;
			CedarAlgorithmId_ = l1Struct.l1Mask[i].ktag.configParams.l1TrigMaskID;
			LavAlgorithmId_ = l1Struct.l1Mask[i].lav.configParams.l1TrigMaskID;
			IrcSacAlgorithmId_ = l1Struct.l1Mask[i].ircsac.configParams.l1TrigMaskID;
			MuvAlgorithmId_ = l1Struct.l1Mask[i].muv.configParams.l1TrigMaskID;
			NewChodAlgorithmId_ = l1Struct.l1Mask[i].newchod.configParams.l1TrigMaskID;
		} else {
			if ((ChodAlgorithmId_ != l1Struct.l1Mask[i].chod.configParams.l1TrigMaskID)
					|| (RichAlgorithmId_ != l1Struct.l1Mask[i].rich.configParams.l1TrigMaskID)
					|| (CedarAlgorithmId_ != l1Struct.l1Mask[i].ktag.configParams.l1TrigMaskID)
					|| (LavAlgorithmId_ != l1Struct.l1Mask[i].lav.configParams.l1TrigMaskID)
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
			LOG_ERROR("Mismatch between NumberOfFlaggedAlgos and algoEnable&FlagMask !!!");
			NumberOfFlaggedAlgos_[i] = NumberOfEnabledAndFlaggedAlgos_[i];
		}

		AlgoLogicMask_[i] = (l1Struct.l1Mask[i].newchod.configParams.l1TrigLogic << NewChodAlgorithmId_)
				| (l1Struct.l1Mask[i].muv.configParams.l1TrigLogic << MuvAlgorithmId_)
				| (l1Struct.l1Mask[i].ircsac.configParams.l1TrigLogic << IrcSacAlgorithmId_)
				| (l1Struct.l1Mask[i].lav.configParams.l1TrigLogic << LavAlgorithmId_)
				| (l1Struct.l1Mask[i].ktag.configParams.l1TrigLogic << CedarAlgorithmId_)
				| (l1Struct.l1Mask[i].chod.configParams.l1TrigLogic << ChodAlgorithmId_);

		AlgoDwScMask_[i] = (l1Struct.l1Mask[i].newchod.configParams.l1TrigDownScale << NewChodAlgorithmId_)
				| (l1Struct.l1Mask[i].muv.configParams.l1TrigDownScale << MuvAlgorithmId_)
				| (l1Struct.l1Mask[i].ircsac.configParams.l1TrigDownScale << IrcSacAlgorithmId_)
				| (l1Struct.l1Mask[i].lav.configParams.l1TrigDownScale << LavAlgorithmId_)
				| (l1Struct.l1Mask[i].ktag.configParams.l1TrigDownScale << CedarAlgorithmId_)
				| (l1Struct.l1Mask[i].chod.configParams.l1TrigDownScale << ChodAlgorithmId_);

		AlgoDwScFactor_[i][(uint) ChodAlgorithmId_] = l1Struct.l1Mask[i].chod.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) RichAlgorithmId_] = l1Struct.l1Mask[i].rich.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) CedarAlgorithmId_] = l1Struct.l1Mask[i].ktag.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) LavAlgorithmId_] = l1Struct.l1Mask[i].lav.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) IrcSacAlgorithmId_] = l1Struct.l1Mask[i].ircsac.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) MuvAlgorithmId_] = l1Struct.l1Mask[i].muv.configParams.l1TrigDSFactor;
		AlgoDwScFactor_[i][(uint) NewChodAlgorithmId_] = l1Struct.l1Mask[i].newchod.configParams.l1TrigDSFactor;

		AlgoProcessID_[i][(uint) ChodAlgorithmId_] = l1Struct.l1Mask[i].chod.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) RichAlgorithmId_] = l1Struct.l1Mask[i].rich.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) CedarAlgorithmId_] = l1Struct.l1Mask[i].ktag.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) LavAlgorithmId_] = l1Struct.l1Mask[i].lav.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) IrcSacAlgorithmId_] = l1Struct.l1Mask[i].ircsac.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) MuvAlgorithmId_] = l1Struct.l1Mask[i].muv.configParams.l1TrigProcessID;
		AlgoProcessID_[i][(uint) NewChodAlgorithmId_] = l1Struct.l1Mask[i].newchod.configParams.l1TrigProcessID;

		CHODAlgo::initialize(i, l1Struct.l1Mask[i].chod);
//		RICHAlgo::initialize(l1Struct.l1Mask[i].rich);
		KtagAlgo::initialize(i, l1Struct.l1Mask[i].ktag);
		LAVAlgo::initialize(i, l1Struct.l1Mask[i].lav);
		IRC_SACAlgo::initialize(i, l1Struct.l1Mask[i].ircsac);
		MUV3Algo::initialize(i, l1Struct.l1Mask[i].muv);
		NewCHODAlgo::initialize(i, l1Struct.l1Mask[i].newchod);

		ChodEnableMask_ |= (l1Struct.l1Mask[i].chod.configParams.l1TrigEnable << i);
		RichEnableMask_ |= (l1Struct.l1Mask[i].rich.configParams.l1TrigEnable << i);
		CedarEnableMask_ |= (l1Struct.l1Mask[i].ktag.configParams.l1TrigEnable << i);
		LavEnableMask_ |= (l1Struct.l1Mask[i].lav.configParams.l1TrigEnable << i);
		IrcSacEnableMask_ |= (l1Struct.l1Mask[i].ircsac.configParams.l1TrigEnable << i);
		MuvEnableMask_ |= (l1Struct.l1Mask[i].muv.configParams.l1TrigEnable << i);
		NewChodEnableMask_ |= (l1Struct.l1Mask[i].newchod.configParams.l1TrigEnable << i);

		ChodFlagMask_ |= (l1Struct.l1Mask[i].chod.configParams.l1TrigFlag << i);
		RichFlagMask_ |= (l1Struct.l1Mask[i].rich.configParams.l1TrigFlag << i);
		CedarFlagMask_ |= (l1Struct.l1Mask[i].ktag.configParams.l1TrigFlag << i);
		LavFlagMask_ |= (l1Struct.l1Mask[i].lav.configParams.l1TrigFlag << i);
		IrcSacFlagMask_ |= (l1Struct.l1Mask[i].ircsac.configParams.l1TrigFlag << i);
		MuvFlagMask_ |= (l1Struct.l1Mask[i].muv.configParams.l1TrigFlag << i);
		NewChodFlagMask_ |= (l1Struct.l1Mask[i].newchod.configParams.l1TrigFlag << i);
	}

	L1DataPacketSize_ = sizeof(L1Global);
	int num = 0;
	int algoNum = 0;
	for (int l0Mask : L0MaskIDs_) {
		NumToMaskID_[num] = l0Mask;
		MaskIDToNum_[l0Mask] = num;
//		LOG_INFO("Initialization of Enabled Masks " << MaskIDToNum[l0Mask] << " " << NumToMaskID[num]);
		L1DataPacketSize_ += (sizeof(L1Mask) + NumberOfEnabledAlgos_[l0Mask] * sizeof(L1Algo));
		for (int i = 0; i != 16; i++) {
			if (AlgoEnableMask_[l0Mask] & (1 << i)) {
				NumToAlgoID_[num][algoNum] = i;
				AlgoIDToNum_[num][i] = algoNum;
//				LOG_INFO("Initialization of Enabled Algos " << AlgoIDToNum[num][i] << " " << NumToAlgoID[num][algoNum]);
				algoNum++;
			}
		}
		num++;
		algoNum = 0;
	}

}

uint_fast8_t L1TriggerProcessor::compute(Event* const event) {

	//using namespace l0;
	uint_fast8_t l1Trigger = 0;
	uint_fast8_t l1GlobalFlagTrigger = 0;
	uint_fast8_t l1MaskFlagTrigger = 0;
	bool isL1Bypassed = 0;
	bool isAllL1AlgoDisable = 0;
	bool isL1WhileTimeout = 0;
	uint_fast8_t numberOfTriggeredL1Masks = 0;
	bool isL0PhysicsTrigger = 0;
	bool isL0PeriodicTrigger = 0;
	bool isL0ControlTrigger = 0;
	bool isReducedEvent = 0;
	bool isAlgoEnableForAllL0Masks = 0;
	uint_fast8_t chodTrigger = 0;
	uint_fast8_t richTrigger = 0;
	uint_fast8_t cedarTrigger = 0;
	uint_fast8_t lavTrigger = 0;
	uint_fast8_t ircsacTrigger = 0;
	uint_fast8_t muvTrigger = 0;
	uint_fast8_t newchodTrigger = 0;
	uint_fast8_t l1TriggerWords[16] = { };
	L1InfoToStorage l1Info;

	DecoderHandler decoder(event);
	//event->readTriggerTypeWordAndFineTime();
	//uint_fast8_t evtRefFineTime = event->getFinetime();

	//L1TriggerProcessor::clear();

	L1InputEvents_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO("L1Event number after adding 1 " << L1InputEvents_);
	L1InputEventsPerBurst_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO("L1Event number per Burst after adding 1 " << L1InputEventsPerBurst_);

//	LOG_INFO("Global FlagMode " << flagMode << " " << L1InputEvents_ << " " << autoFlagFactor);

	if (FlagMode_) {
		l1GlobalFlagTrigger = 1;
	} else if (AutoFlagFactor_ && (L1InputEvents_ % AutoFlagFactor_ == 0)) {
		l1GlobalFlagTrigger = 1;
	}
//	LOG_INFO("l1GlobalFlagTrigger " << (uint)l1GlobalFlagTrigger);
	/*
	 * Check if the event should bypass the processing
	 */
	if (event->isSpecialTriggerEvent()) {
		isL1Bypassed = 1;
		event->setRrequestZeroSuppressedCreamData(false);
		l1Trigger = ((uint) l1GlobalFlagTrigger << 7) | ((l1MaskFlagTrigger != 0) << 6) | (isL1Bypassed << 5) | (isAllL1AlgoDisable << 4)
				| (numberOfTriggeredL1Masks != 0);
		L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed);
//		printf("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger %8x\n",l1Trigger);
		L1TriggerProcessor::writeL1Data(event, l1TriggerWords, &l1Info);
//		L1TriggerProcessor::readL1Data(event);
		return l1Trigger;
	}
	if (event->isPulserGTKTriggerEvent() || bypassEvent()) {
		isL1Bypassed = 1;
		event->setRrequestZeroSuppressedCreamData(true);
		l1Trigger = ((uint) l1GlobalFlagTrigger << 7) | ((l1MaskFlagTrigger != 0) << 6) | (isL1Bypassed << 5) | (isAllL1AlgoDisable << 4)
				| (numberOfTriggeredL1Masks != 0);
		L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed);
//		printf("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger %8x\n",l1Trigger);
		L1TriggerProcessor::writeL1Data(event, l1TriggerWords, &l1Info);
//		L1TriggerProcessor::readL1Data(event);
		return l1Trigger;
	}

	/*
	 * Check if the event fulfills the reduction option
	 *
	 */
//	LOG_INFO("L1Reduction Factor " << reductionFactor);
//	LOG_INFO("Modulo " << L1InputEvents_ % reductionFactor);
	if (ReductionFactor_ && (L1InputEvents_ % ReductionFactor_ != 0))
		return 0;

	L1InputReducedEvents_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO("L1ReducedEvent number after adding 1 " << L1InputReducedEvents_);

	/*
	 * The event is ready to be processed
	 *
	 */
	//l0TrigWord = event->getL0TriggerTypeWord();
//	LOG_INFO("l0TriggerWord " << std::hex << (uint) l0TrigWord << std::dec);
	//l0DataType = event->getTriggerDataType();
//	LOG_INFO("l0TriggerDataType " << std::hex << (uint) l0DataType << std::dec);
	/*
	 * Store the global event timestamp taken from the reverence detector
	 */
	l0::MEPFragment* tsFragment = event->getL0SubeventBySourceIDNum(SourceIDManager::TS_SOURCEID_NUM)->getFragment(0);
	event->setTimestamp(tsFragment->getTimestamp());

	uint_fast8_t l1TriggerTmp;
	uint_fast8_t l1FlagTrigger;
	uint watchingWhileLoops = 0;

	if (event->isPhysicsTriggerEvent()) {
		isL0PhysicsTrigger = 1;
	}
	if (event->isPeriodicTriggerEvent()) {
		isL0PeriodicTrigger = 1;
		isL1Bypassed = 1;
	}
	if (event->isControlTriggerEvent()) {
		isL0ControlTrigger = 1;
		isL1Bypassed = 1;
	}
	uint_fast16_t l0TrigFlags = event->getTriggerFlags();
	if (isL0PhysicsTrigger) {
		for (int i = 0; i != 16; i++) {
			l1TriggerWords[i] = 0;
			if (l0TrigFlags & (1 << i)) {
				L1InputReducedEventsPerL0Mask_[i].fetch_add(1, std::memory_order_relaxed);
				uint_fast32_t l1ProcessID = 0;
				watchingWhileLoops = 0;
				l1TriggerTmp = 0;
				l1FlagTrigger = 0;

//				LOG_INFO("NumberOfEnabledAndFlaggedAlgos_[" << i << "] " << NumberOfEnabledAndFlaggedAlgos_[i]);
				if (NumberOfEnabledAndFlaggedAlgos_[i])
					l1FlagTrigger = 1;
//				LOG_INFO("Reduction factor " << maskReductionFactor[i] << " Modulo (maskReductionFactor) " << L1InputReducedEventsPerL0Mask_[i] % maskReductionFactor[i]);
				if (MaskReductionFactor_[i] && (L1InputReducedEventsPerL0Mask_[i] % MaskReductionFactor_[i] != 0))
					isReducedEvent = 1;
				else
					isReducedEvent = 0;

//				LOG_INFO("isReducedEvent " << isReducedEvent);
//				LOG_INFO("i " << i << " processID " << l1ProcessID << " nEnAlgos " << NumberOfEnabledAlgos_[i] << " l1TriggerTemp " << (uint) l1TriggerTmp << " nEnableFlagAlgos " << NumberOfEnabledAndFlaggedAlgos_[i] << " l1FlagTrig " << (uint) l1FlagTrigger);
				if (!NumberOfEnabledAlgos_[i])
					isAllL1AlgoDisable = 1;

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
							l1Info.setL1CHODTrgWrd(chodTrigger);
						} else {
							l1TriggerTmp |= (not chodTrigger << (uint) ChodAlgorithmId_);
							l1Info.setL1CHODTrgWrd(not chodTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
					if ((ChodEnableMask_ & l0TrigFlags) == l0TrigFlags)
						isAlgoEnableForAllL0Masks = 1;
//					if (!(l1TriggerTmp & AlgoEnableMask_[i]) && l1ProcessID && !((ChodEnableMask_ & ChodFlagMask_) & (1 << i))) {
					if (((AlgoEnableMask_[i] & (1 << ChodAlgorithmId_)) && !(l1TriggerTmp & (1 << ChodAlgorithmId_))) && l1ProcessID
							&& !((ChodEnableMask_ & ChodFlagMask_) & (1 << i))) {
						watchingWhileLoops++;
						break;
					}
					/*
					 if ((richEnableMask & (1 << i))
					 && richProcessID[i] == l1ProcessID
					 && SourceIDManager::isRhichActive()) {
					 LOG_INFO("I am Processing RICH!!!!!");
					 richTrigger = RICHAlgo::processRICHTrigger(decoder);
					 //					if (richTrigger != 0) {
					 //						L1Downscaling::processAlgorithm(richAlgorithmId);
					 //					}
					 l1ProcessID++;
					 l1TriggerTmp = richTrigger;
					 //printf("L1TriggerProcessor.cpp: richTrigger %d\n",
					 richTrigger);
					 }
					 if ((richEnableMask & l0TrigFlags) == l0TrigFlags)
					 isAlgoEnableForAllL0Masks = 1;
					 if (!(l1TriggerTmp & algoEnableMask[i]) && l1ProcessID
					 && !numberOfFlaggedAlgos[i])
					 break;
					 */

					if ((CedarEnableMask_ & (1 << i)) && AlgoProcessID_[i][CedarAlgorithmId_] == l1ProcessID
							&& SourceIDManager::isCedarActive()) {
						if (!l1Info.isL1KTAGProcessed()) {
							cedarTrigger = KtagAlgo::processKtagTrigger(i, decoder, &l1Info);
//							if (cedarTrigger != 0) {
//								L1Downscaling::processAlgorithm (cedarAlgorithmId);
//							}
						}
						l1ProcessID++;
//						printf("L1TriggerProcessor.cpp: cedarTrigger %d\n", cedarTrigger);

						if (AlgoLogicMask_[i] & (1 << (uint) CedarAlgorithmId_)) {
							l1TriggerTmp |= (cedarTrigger << (uint) CedarAlgorithmId_);
							l1Info.setL1KTAGTrgWrd(cedarTrigger);
						} else {
							l1TriggerTmp |= (not cedarTrigger << (uint) CedarAlgorithmId_);
							l1Info.setL1KTAGTrgWrd(not cedarTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
					if ((CedarEnableMask_ & l0TrigFlags) == l0TrigFlags)
						isAlgoEnableForAllL0Masks = 1;
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
//							if (lavTrigger != 0) {
//								L1Downscaling::processAlgorithm (lavAlgorithmId);
//							}
						}
						l1ProcessID++;
//						printf("L1TriggerProcessor.cpp: lavTrigger %d\n", lavTrigger);

						if (AlgoLogicMask_[i] & (1 << (uint) LavAlgorithmId_)) {
							l1TriggerTmp |= (lavTrigger << (uint) LavAlgorithmId_);
							l1Info.setL1LAVTrgWrd(lavTrigger);
						} else {
							l1TriggerTmp |= (not lavTrigger << (uint) LavAlgorithmId_);
							l1Info.setL1LAVTrgWrd(not lavTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
					if ((LavEnableMask_ & l0TrigFlags) == l0TrigFlags)
						isAlgoEnableForAllL0Masks = 1;
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
//							if (ircsacTrigger != 0) {
//								L1Downscaling::processAlgorithm (ircsacAlgorithmId);
//							}
						}
						l1ProcessID++;
//						printf("L1TriggerProcessor.cpp: ircsacTrigger %d\n", ircsacTrigger);

						if (AlgoLogicMask_[i] & (1 << (uint) IrcSacAlgorithmId_)) {
							l1TriggerTmp |= (ircsacTrigger << (uint) IrcSacAlgorithmId_);
							l1Info.setL1IRCSACTrgWrd(ircsacTrigger);
						} else {
							l1TriggerTmp |= (not ircsacTrigger << (uint) IrcSacAlgorithmId_);
							l1Info.setL1IRCSACTrgWrd(not ircsacTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
					if ((IrcSacEnableMask_ & l0TrigFlags) == l0TrigFlags)
						isAlgoEnableForAllL0Masks = 1;
//					if (!(l1TriggerTmp & AlgoEnableMask_[i]) && l1ProcessID && !((IrcSacEnableMask_ & IrcSacFlagMask_) & (1 << i))) {
					if (((AlgoEnableMask_[i] & (1 << IrcSacAlgorithmId_)) && !(l1TriggerTmp & (1 << IrcSacAlgorithmId_))) && l1ProcessID
							&& !((IrcSacEnableMask_ & IrcSacFlagMask_) & (1 << i))) {
						watchingWhileLoops++;
						break;
					}

					if ((MuvEnableMask_ & (1 << i)) && AlgoProcessID_[i][MuvAlgorithmId_] == l1ProcessID
							&& SourceIDManager::isMUV3Active()) {
						if (!l1Info.isL1MUV3Processed()) {
							muvTrigger = MUV3Algo::processMUV3Trigger0(i, decoder, &l1Info);
//							if (muvTrigger != 0) {
//								L1Downscaling::processAlgorithm(muvAlgorithmId);
//							}
						}
						l1ProcessID++;
//						printf("L1TriggerProcessor.cpp: muvTrigger %d\n", muvTrigger);

						if (AlgoLogicMask_[i] & (1 << (uint) MuvAlgorithmId_)) {
							l1TriggerTmp |= (muvTrigger << (uint) MuvAlgorithmId_);
							l1Info.setL1MUV3TrgWrd(muvTrigger);
						} else {
							l1TriggerTmp |= (not muvTrigger << (uint) MuvAlgorithmId_);
							l1Info.setL1MUV3TrgWrd(not muvTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
					if ((MuvEnableMask_ & l0TrigFlags) == l0TrigFlags)
						isAlgoEnableForAllL0Masks = 1;
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
//							if (newchodTrigger != 0) {
//								L1Downscaling::processAlgorithm(NewChodAlgorithmId_);
//							}
						}
						l1ProcessID++;
//						printf("L1TriggerProcessor.cpp: newchodTrigger %d\n", newchodTrigger);

						if (AlgoLogicMask_[i] & (1 << (uint) NewChodAlgorithmId_)) {
							l1TriggerTmp |= (newchodTrigger << (uint) NewChodAlgorithmId_);
							l1Info.setL1NewCHODTrgWrd(newchodTrigger);
						} else {
							l1TriggerTmp |= (not newchodTrigger << (uint) NewChodAlgorithmId_);
							l1Info.setL1NewCHODTrgWrd(not newchodTrigger);
						}
//						printf("L1TriggerProcessor.cpp: tmpTrigger %d\n", l1TriggerTmp);
					}
					if ((NewChodEnableMask_ & l0TrigFlags) == l0TrigFlags)
						isAlgoEnableForAllL0Masks = 1;
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

//				LOG_INFO("watchingWhileLoops " << watchingWhileLoops);
//				LOG_INFO("isReducedEvent " << isReducedEvent);
//				LOG_INFO("l1ProcessID " << l1ProcessID);
//				LOG_INFO("numberOfEnabledAlgos " << numberOfEnabledAlgos[i]);

//				printf("L1TriggerProcessor.cpp: l1Trigger (!!TMP!!) %x\n", l1TriggerTmp);
//				LOG_INFO("isReducedEvent " << isReducedEvent);
//				LOG_INFO("EnableMask " << AlgoEnableMask_[i] << " trigTmp & EnableMask " << (l1TriggerTmp & AlgoEnableMask_[i]));
//				LOG_INFO(" l1FlagTrig " << (uint) l1FlagTrigger);
//				LOG_INFO("trigTemp & EnableCutMask " << (l1TriggerTmp & AlgoEnableCutMask_[i]));
				if (!isReducedEvent
						&& (!AlgoEnableMask_[i] || ((l1TriggerTmp & AlgoEnableMask_[i]) == AlgoEnableMask_[i])
								|| ((l1TriggerTmp & AlgoEnableCutMask_[i]) == AlgoEnableCutMask_[i]))) {
					L1AcceptedEventsPerL0Mask_[i].fetch_add(1, std::memory_order_relaxed);

//					LOG_INFO("L1 Accepted Event Per L0 mask " << i << " number after adding 1 " << L1AcceptedEventsPerL0Mask_[i]);
//					LOG_INFO("TrigTmp " << (uint) l1TriggerTmp << " dwscMask " << (uint) algoDwScMask[i]);
//					LOG_INFO("downscale Factor " << algoDwScFactor[i][__builtin_ctz( (uint) algoDwScMask[i])]);
//					LOG_INFO("Modulo " << L1AcceptedEventsPerL0Mask_[i] % algoDwScFactor[i][__builtin_ctz( (uint) algoDwScMask[i])]);

					l1MaskFlagTrigger += l1FlagTrigger;
//					LOG_INFO("l1MaskFlagTrigger " << (uint)l1MaskFlagTrigger);

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
						l1TriggerWords[i] = 0;
					} else
						l1TriggerWords[i] = ((l1TriggerTmp & AlgoEnableMask_[i]) == AlgoEnableMask_[i]);
				} else
					l1TriggerWords[i] = 0;
//				printf("L1TriggerProcessor.cpp: l1Trigger %x\n", l1TriggerWords[i]);
			}
			if (__builtin_popcount((uint) l1TriggerWords[i]))
				numberOfTriggeredL1Masks++;
		}

//		printf("Summary of Triggered Masks: %d\n", numberOfTriggeredL1Masks);
//		for (int i = 0; i != 16; i++) printf("Summary of Trigger Words: l1Trigger %x\n",l1TriggerWords[i]);
	}

	/*
	 * Reduction of specific trigger algorithms
	 *
	 */
	/*
	 if (L1Reduction::processAlgorithm(cedarAlgorithmId)) {
	 if (SourceIDManager::isCedarActive()) {
	 cedarTrigger = KtagAlgo::processKtagTrigger(decoder);
	 }
	 }
	 if (L1Reduction::processAlgorithm(chodAlgorithmId)) {
	 if (SourceIDManager::isChodActive()) {
	 chodTrigger = CHODAlgo::processCHODTrigger(decoder);
	 }
	 }
	 if (L1Reduction::processAlgorithm(richAlgorithmId)) {
	 if (SourceIDManager::isRhichActive()) {
	 richTrigger = RICHAlgo::processRICHTrigger(decoder);
	 }
	 }
	 */

	/*
	 * Final L1 trigger word calculation
	 */

	l1Trigger = ((uint) l1GlobalFlagTrigger << 7) | ((l1MaskFlagTrigger != 0) << 6) | (isL1Bypassed << 5) | (isAllL1AlgoDisable << 4)
			| (isL1WhileTimeout << 3) | (numberOfTriggeredL1Masks != 0);

	if (l1Trigger != 0) {
		/*
		 * TODO: Pass this counter to Nicolas to print number of events before downscaling
		 */
		L1AcceptedEvents_.fetch_add(1, std::memory_order_relaxed);
//		LOG_INFO("L1 Accepted Event number after adding 1 " << L1AcceptedEvents_);

//Global L1 downscaling
//		LOG_INFO("L1Downscale Factor " << downscaleFactor);
//		LOG_INFO("Modulo " << L1AcceptedEvents_ % downscaleFactor);
		if (DownscaleFactor_ && (L1AcceptedEvents_ % DownscaleFactor_ != 0))
			return 0;
		/*
		 * Decision whether or not to request zero suppressed data from the creams
		 */
		event->setRrequestZeroSuppressedCreamData(true);
		event->setProcessingID(0); // 0 indicates raw data as collected from the detector
		L1TriggerProcessor::writeL1Data(event, l1TriggerWords, &l1Info, isL1WhileTimeout);
//		L1TriggerProcessor::readL1Data(event);

	}

//	std::bitset<8> l1TrgWrd(l1Trigger);
//	printf("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger %8x\n", l1Trigger);
//	LOG_INFO("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger " << l1TrgWrd);
	L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed); // The second 8 bits are the L1 trigger type word
	return l1Trigger;
}

void L1TriggerProcessor::readL1Data(Event* const event) {
	const l0::MEPFragment* const L1TPEvent = event->getL1ResultSubevent()->getFragment(0);
	char* pload = (char*) L1TPEvent->getPayload();
	uint_fast16_t length = (uint_fast16_t) L1TPEvent->getPayloadLength();
	if ((uint) length != (uint) L1DataPacketSize_)
		LOG_ERROR("length (in bytes) " << (uint)length << " is different from l1DataPacketsSize " << (uint)L1DataPacketSize_);
	uint32_t dataWord;
	for (uint i = 0; i != (uint) L1DataPacketSize_ / 4; i++) {
		L1Data* dataPacket = (L1Data*) pload + i;
		std::bitset<32> data((uint) dataPacket->dataWords);
		LOG_INFO("L1 Data " << data);
	}
}
void L1TriggerProcessor::writeL1Data(Event* const event, const uint_fast8_t* l1TriggerWords, L1InfoToStorage* l1Info,
		bool isL1WhileTimeout) {

	uint nBlockHeaderWords = 0;
	uint_fast8_t refTimeSourceID = 0;
	uint_fast8_t refTimeSourceID_tmp = 0;
	uint_fast16_t l0TrigFlags = event->getTriggerFlags();

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
	globalPacket->format = 0;
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
	for (int iNum = 0; iNum < NumberOfEnabledL0Masks_; iNum++) {
		if (NumToMaskID_[iNum] == -1)
			LOG_ERROR("ERROR! Wrong association of mask ID!");
		else
			numToMaskID = NumToMaskID_[iNum];

		L1Mask* maskPacket = (L1Mask*) (payload + nBlockHeaderWords + nMaskWords);

//		LOG_INFO("NumToMaskID " << numToMaskID);
		maskPacket->numberOfEnabledAlgos = NumberOfEnabledAlgos_[numToMaskID];
		maskPacket->triggerWord = l1TriggerWords[numToMaskID];
		maskPacket->maskID = numToMaskID;
		maskPacket->flags = 0;
		if (NumberOfEnabledAndFlaggedAlgos_[numToMaskID])
			maskPacket->flags |= (1 << 6);
		if (!NumberOfEnabledAlgos_[numToMaskID])
			maskPacket->flags |= (1 << 4);
		if (isL1WhileTimeout)
			maskPacket->flags |= (1 << 2);
		maskPacket->reductionFactor = MaskReductionFactor_[numToMaskID];
		maskPacket->reserved = 0;

		nMaskWords += sizeof(L1Mask); //2 32-bit header words for each mask

//		LOG_INFO("maskID " << (uint) maskPacket->maskID << " triggerWord " << (uint) maskPacket->triggerWord);
//		LOG_INFO("numberOfEnabledAlgos " << (uint)maskPacket->numberOfEnabledAlgos);
//		LOG_INFO("reductionFactor " << (uint)maskPacket->reductionFactor);

//		std::bitset<8> maskFlag((uint) maskPacket->flags);
//		LOG_INFO("Flags " << (uint)maskPacket->flags << " " << maskFlag);

		for (int iAlgoNum = 0; iAlgoNum < NumberOfEnabledAlgos_[numToMaskID]; iAlgoNum++) {
			if (NumToAlgoID_[iNum][iAlgoNum] == -1)
				LOG_ERROR("ERROR! Wrong association of algo ID!");
			else
				numToAlgoID = NumToAlgoID_[iNum][iAlgoNum];

			L1Algo* algoPacket = (L1Algo*) (payload + nBlockHeaderWords + nMaskWords);

			if (l0TrigFlags & (1 << numToMaskID)) {

//				LOG_INFO("NumToAlgoID " << numToAlgoID);
				algoPacket->processID = AlgoProcessID_[numToMaskID][numToAlgoID];
				algoPacket->algoID = numToAlgoID;
				algoPacket->downscaleFactor = AlgoDwScFactor_[numToMaskID][numToAlgoID];

//				LOG_INFO("Enable " << ((AlgoEnableMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
//				LOG_INFO("Flag " << ((AlgoFlagMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
//				LOG_INFO("Logic " << ((AlgoLogicMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
//				LOG_INFO("DwSc " << ((AlgoDwScMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));

				algoPacket->algoFlags = (((AlgoEnableMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID) << 6)
						| (((AlgoFlagMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID) << 4)
						| (((AlgoLogicMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID) << 2)
						| (((AlgoDwScMask_[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));

				L1TriggerProcessor::writeAlgoPacket(numToAlgoID, algoPacket, numToMaskID, l1Info);

				refTimeSourceID_tmp = (algoPacket->qualityFlags & 0x3);
				if (refTimeSourceID != refTimeSourceID_tmp)
					refTimeSourceID = refTimeSourceID_tmp;
			} else
				std::memset(algoPacket, 0, sizeof(L1Algo));

			algoPacket->numberOfWords = (sizeof(L1Algo) / 4.);

			nMaskWords += sizeof(L1Algo);

//			LOG_INFO("Algo ID " << (uint) algoPacket->algoID << " Algo ProcessID " << (uint)algoPacket->processID);
//			LOG_INFO("Quality Flags " << (uint) algoPacket->qualityFlags << " Algo Flags " << (uint)algoPacket->algoFlags);

//			std::bitset<8> algoFlag((uint) algoPacket->algoFlags);
//			std::bitset<8> qualityFlag((uint) algoPacket->qualityFlags);
//			LOG_INFO("Quality Flags " << qualityFlag << " Algo Flags " << algoFlag);

//			LOG_INFO("Number of Words " << (uint) algoPacket->numberOfWords << " DS Factor " << (uint)algoPacket->downscaleFactor);
//			LOG_INFO("Online TW " << (uint)algoPacket->onlineTimeWindow);
//			LOG_INFO("L1 Data[0] " << (uint) algoPacket->l1Data[0] << " L1 Data[1] " << (uint)algoPacket->l1Data[1]);

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
	case 5:
//		StrawAlgo::writeData(algoPacket,l0MaskID);
		return true;
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
