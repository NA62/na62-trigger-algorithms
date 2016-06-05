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
#include "MUVAlgo.h"

namespace na62 {

std::atomic<uint64_t>* L1TriggerProcessor::L1Triggers_ = new std::atomic<
		uint64_t>[0xFF + 1];
std::atomic<uint64_t>* L1TriggerProcessor::L1AcceptedEventsPerL0Mask_ =
		new std::atomic<uint64_t>[16];
std::atomic<uint64_t>* L1TriggerProcessor::L1InputReducedEventsPerL0Mask_ =
		new std::atomic<uint64_t>[16];
std::atomic<uint64_t>** L1TriggerProcessor::eventCountersByL0MaskByAlgoID_;
std::atomic<uint64_t> L1TriggerProcessor::L1InputEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1InputReducedEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1InputEventsPerBurst_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1AcceptedEvents_(0);

double L1TriggerProcessor::bypassProbability;
uint L1TriggerProcessor::reductionFactor = 0;
uint L1TriggerProcessor::downscaleFactor = 0;
bool L1TriggerProcessor::flagMode = 0;
uint L1TriggerProcessor::autoFlagFactor = 0;
uint L1TriggerProcessor::referenceTimeSourceID = 0;

uint L1TriggerProcessor::numberOfEnabledAlgos[16];
uint L1TriggerProcessor::numberOfFlaggedAlgos[16];
uint L1TriggerProcessor::numberOfEnabledAndFlaggedAlgos[16];
uint L1TriggerProcessor::maskReductionFactor[16];
uint_fast16_t L1TriggerProcessor::algoEnableMask[16];
uint_fast16_t L1TriggerProcessor::algoFlagMask[16];
uint_fast16_t L1TriggerProcessor::algoLogicMask[16];
uint_fast16_t L1TriggerProcessor::algoDwScMask[16];
uint16_t L1TriggerProcessor::algoDwScFactor[16][6];
uint8_t L1TriggerProcessor::algoProcessID[16][6];

uint_fast16_t L1TriggerProcessor::chodEnableMask = 0;
uint_fast16_t L1TriggerProcessor::richEnableMask = 0;
uint_fast16_t L1TriggerProcessor::cedarEnableMask = 0;
uint_fast16_t L1TriggerProcessor::lavEnableMask = 0;
uint_fast16_t L1TriggerProcessor::ircsacEnableMask = 0;
uint_fast16_t L1TriggerProcessor::muvEnableMask = 0;

uint_fast16_t L1TriggerProcessor::chodFlagMask = 0;
uint_fast16_t L1TriggerProcessor::richFlagMask = 0;
uint_fast16_t L1TriggerProcessor::cedarFlagMask = 0;
uint_fast16_t L1TriggerProcessor::lavFlagMask = 0;
uint_fast16_t L1TriggerProcessor::ircsacFlagMask = 0;
uint_fast16_t L1TriggerProcessor::muvFlagMask = 0;

int L1TriggerProcessor::chodProcessID[16];
int L1TriggerProcessor::richProcessID[16];
int L1TriggerProcessor::cedarProcessID[16];
int L1TriggerProcessor::lavProcessID[16];
int L1TriggerProcessor::ircsacProcessID[16];
int L1TriggerProcessor::muvProcessID[16];

uint L1TriggerProcessor::chodAlgorithmId;
uint L1TriggerProcessor::richAlgorithmId;
uint L1TriggerProcessor::cedarAlgorithmId;
uint L1TriggerProcessor::lavAlgorithmId;
uint L1TriggerProcessor::ircsacAlgorithmId;
uint L1TriggerProcessor::muvAlgorithmId;

uint_fast8_t L1TriggerProcessor::l0TrigWord = 0;
uint_fast8_t L1TriggerProcessor::l0DataType = 0;
uint_fast16_t L1TriggerProcessor::l0TrigFlags = 0;

uint_fast8_t L1TriggerProcessor::chodTrigger = 0;
uint_fast8_t L1TriggerProcessor::richTrigger = 0;
uint_fast8_t L1TriggerProcessor::cedarTrigger = 0;
uint_fast8_t L1TriggerProcessor::lavTrigger = 0;
uint_fast8_t L1TriggerProcessor::ircsacTrigger = 0;
uint_fast8_t L1TriggerProcessor::muvTrigger = 0;
uint_fast8_t L1TriggerProcessor::l1TriggerWords[16];

uint L1TriggerProcessor::MaskIDToNum[16];
uint L1TriggerProcessor::NumToMaskID[16];

uint_fast8_t L1TriggerProcessor::evtRefFineTime;

L1InfoToStorage* L1TriggerProcessor::l1Info_ = L1InfoToStorage::GetInstance();
uint L1TriggerProcessor::l1ProcessID = 0;
bool L1TriggerProcessor::isL0PhysicsTrigger = 0;
bool L1TriggerProcessor::isL0PeriodicTrigger = 0;
bool L1TriggerProcessor::isL0ControlTrigger = 0;
bool L1TriggerProcessor::isL1Bypassed = 0;
uint L1TriggerProcessor::numberOfTriggeredL1Masks = 0;
bool L1TriggerProcessor::isAlgoEnableForAllL0Masks = 0;
bool L1TriggerProcessor::isReducedEvent = 0;
bool L1TriggerProcessor::isAllL1AlgoDisable = 0;
uint_fast8_t L1TriggerProcessor::numberOfEnabledL0Masks = 0;

std::vector<int> L1TriggerProcessor::l0MaskIDs;
uint_fast32_t L1TriggerProcessor::L1DataPacketLength = 0;
L1Block* L1TriggerProcessor::l1Block;

void L1TriggerProcessor::registerReductionAlgorithms() {
	uint numberOfRegisteredAlgorithms = 0;
//	cedarAlgorithmId = L1Reduction::registerAlgorithm("CEDAR");
//	chodAlgorithmId = L1Reduction::registerAlgorithm("CHOD");
//	richAlgorithmId = L1Reduction::registerAlgorithm("RICH");
//	richAlgorithmId = L1Reduction::registerAlgorithm("LAV");
}

void L1TriggerProcessor::registerDownscalingAlgorithms() {
	uint numberOfRegisteredAlgorithms = 0;
//	cedarAlgorithmId = L1Downscaling::registerAlgorithm("CEDAR");
//	chodAlgorithmId = L1Downscaling::registerAlgorithm("CHOD");
//	richAlgorithmId = L1Downscaling::registerAlgorithm("RICH");
//	lavAlgorithmId = L1Downscaling::registerAlgorithm("LAV");
}

bool L1TriggerProcessor::isRequestZeroSuppressedCreamData(
		uint_fast8_t l1TriggerTypeWord) {
	// add here any special L1 trigger word requiring NZS LKr data
	return l1TriggerTypeWord != TRIGGER_L1_SPECIAL;
}

void L1TriggerProcessor::initialize(l1Struct &l1Struct) {
	// Seed for rand()
	srand(time(NULL));

	l0MaskIDs = TriggerOptions::GetIntList(OPTION_ACTIVE_L0_MASKS);
	numberOfEnabledL0Masks = l0MaskIDs.size();
//	LOG_INFO("numberOfEnabledL0Masks " << (uint)numberOfEnabledL0Masks);

	L1DataPacketLength = sizeof(L1Global)
			+ numberOfEnabledL0Masks * sizeof(L1Mask);
	char* payload = new char[(uint) L1DataPacketLength];
	l1Block = (L1Block *) (payload);

	for (int i = 0; i != 0xFF + 1; i++) {
		L1Triggers_[i] = 0;
	}
	eventCountersByL0MaskByAlgoID_ = new std::atomic<uint64_t>*[16];
	for (int i = 0; i != 16; ++i) {
		eventCountersByL0MaskByAlgoID_[i] = new std::atomic<uint64_t>[6] { };
		L1AcceptedEventsPerL0Mask_[i] = 0;
		L1InputReducedEventsPerL0Mask_[i] = 0;
		for (int j = 0; j != 6; ++j) {
			eventCountersByL0MaskByAlgoID_[i][j] = 0;
		}
	}

	bypassProbability = l1Struct.l1Global.l1BypassProbability;
	reductionFactor = l1Struct.l1Global.l1ReductionFactor;
	downscaleFactor = l1Struct.l1Global.l1DownscaleFactor;
	flagMode = (bool) l1Struct.l1Global.l1FlagMode;
	autoFlagFactor = l1Struct.l1Global.l1AutoFlagFactor;
	referenceTimeSourceID = l1Struct.l1Global.l1ReferenceTimeSourceID;

	L1Downscaling::initialize();
	L1Reduction::initialize();

	for (int i = 0; i != 16; i++) {
		/*
		 * Initialisation of 16 L0 trigger masks
		 */
		NumToMaskID[i] = -1;
		MaskIDToNum[i] = -1;
		l1TriggerWords[i] = 0;

		if (!i) {
			chodAlgorithmId = l1Struct.l1Mask[i].chod.configParams.l1TrigMaskID;
			richAlgorithmId = l1Struct.l1Mask[i].rich.configParams.l1TrigMaskID;
			cedarAlgorithmId =
					l1Struct.l1Mask[i].ktag.configParams.l1TrigMaskID;
			lavAlgorithmId = l1Struct.l1Mask[i].lav.configParams.l1TrigMaskID;
			ircsacAlgorithmId =
					l1Struct.l1Mask[i].ircsac.configParams.l1TrigMaskID;
			muvAlgorithmId = l1Struct.l1Mask[i].muv.configParams.l1TrigMaskID;
		} else {
			if ((chodAlgorithmId
					!= l1Struct.l1Mask[i].chod.configParams.l1TrigMaskID)
					|| (richAlgorithmId
							!= l1Struct.l1Mask[i].rich.configParams.l1TrigMaskID)
					|| (cedarAlgorithmId
							!= l1Struct.l1Mask[i].ktag.configParams.l1TrigMaskID)
					|| (lavAlgorithmId
							!= l1Struct.l1Mask[i].lav.configParams.l1TrigMaskID)
					|| (ircsacAlgorithmId
							!= l1Struct.l1Mask[i].ircsac.configParams.l1TrigMaskID)
					|| (muvAlgorithmId
							!= l1Struct.l1Mask[i].muv.configParams.l1TrigMaskID))
				LOG_ERROR("Mismatch between AlgoID !!!");
			// Throw Exception!
		}
		numberOfEnabledAlgos[i] = l1Struct.l1Mask[i].numberOfEnabledAlgos;
		numberOfFlaggedAlgos[i] = l1Struct.l1Mask[i].numberOfFlaggedAlgos;
		maskReductionFactor[i] = l1Struct.l1Mask[i].maskReductionFactor;

		algoEnableMask[i] = (l1Struct.l1Mask[i].muv.configParams.l1TrigEnable
				<< muvAlgorithmId)
				| (l1Struct.l1Mask[i].ircsac.configParams.l1TrigEnable
						<< ircsacAlgorithmId)
				| (l1Struct.l1Mask[i].lav.configParams.l1TrigEnable
						<< lavAlgorithmId)
				| (l1Struct.l1Mask[i].ktag.configParams.l1TrigEnable
						<< cedarAlgorithmId)
				| (l1Struct.l1Mask[i].chod.configParams.l1TrigEnable
						<< chodAlgorithmId);
		if (numberOfEnabledAlgos[i]
				!= __builtin_popcount((uint) algoEnableMask[i])) {
			LOG_ERROR(
					"Mismatch between NumberOfEnabledAlgos and algoEnableMask !!!");
			numberOfEnabledAlgos[i] = __builtin_popcount(
					(uint) algoEnableMask[i]);
		}

		algoFlagMask[i] = (l1Struct.l1Mask[i].muv.configParams.l1TrigFlag
				<< muvAlgorithmId)
				| (l1Struct.l1Mask[i].ircsac.configParams.l1TrigFlag
						<< ircsacAlgorithmId)
				| (l1Struct.l1Mask[i].lav.configParams.l1TrigFlag
						<< lavAlgorithmId)
				| (l1Struct.l1Mask[i].ktag.configParams.l1TrigFlag
						<< cedarAlgorithmId)
				| (l1Struct.l1Mask[i].chod.configParams.l1TrigFlag
						<< chodAlgorithmId);
		if (numberOfFlaggedAlgos[i]
				!= __builtin_popcount((uint) algoFlagMask[i])) {
			LOG_ERROR(
					"Mismatch between NumberOfFlaggedAlgos and algoFlagMask !!!");
			numberOfFlaggedAlgos[i] = __builtin_popcount(
					(uint) algoFlagMask[i]);
		}

		numberOfEnabledAndFlaggedAlgos[i] = __builtin_popcount(
				(uint) (algoEnableMask[i] & algoFlagMask[i]));
//		LOG_INFO("number of Enabled algo " << numberOfEnabledAlgos[i] << " Flagged " << numberOfFlaggedAlgos[i] << " EnabledAndFlagged " << numberOfEnabledAndFlaggedAlgos[i]);

		algoLogicMask[i] = (l1Struct.l1Mask[i].muv.configParams.l1TrigLogic
				<< muvAlgorithmId)
				| (l1Struct.l1Mask[i].ircsac.configParams.l1TrigLogic
						<< ircsacAlgorithmId)
				| (l1Struct.l1Mask[i].lav.configParams.l1TrigLogic
						<< lavAlgorithmId)
				| (l1Struct.l1Mask[i].ktag.configParams.l1TrigLogic
						<< cedarAlgorithmId)
				| (l1Struct.l1Mask[i].chod.configParams.l1TrigLogic
						<< chodAlgorithmId);

		algoDwScMask[i] = (l1Struct.l1Mask[i].muv.configParams.l1TrigDownScale
				<< muvAlgorithmId)
				| (l1Struct.l1Mask[i].ircsac.configParams.l1TrigDownScale
						<< ircsacAlgorithmId)
				| (l1Struct.l1Mask[i].lav.configParams.l1TrigDownScale
						<< lavAlgorithmId)
				| (l1Struct.l1Mask[i].ktag.configParams.l1TrigDownScale
						<< cedarAlgorithmId)
				| (l1Struct.l1Mask[i].chod.configParams.l1TrigDownScale
						<< chodAlgorithmId);

		algoDwScFactor[i][(uint) chodAlgorithmId] =
				l1Struct.l1Mask[i].chod.configParams.l1TrigDSFactor;
		algoDwScFactor[i][(uint) richAlgorithmId] =
				l1Struct.l1Mask[i].rich.configParams.l1TrigDSFactor;
		algoDwScFactor[i][(uint) cedarAlgorithmId] =
				l1Struct.l1Mask[i].ktag.configParams.l1TrigDSFactor;
		algoDwScFactor[i][(uint) lavAlgorithmId] =
				l1Struct.l1Mask[i].lav.configParams.l1TrigDSFactor;
		algoDwScFactor[i][(uint) ircsacAlgorithmId] =
				l1Struct.l1Mask[i].ircsac.configParams.l1TrigDSFactor;
		algoDwScFactor[i][(uint) muvAlgorithmId] =
				l1Struct.l1Mask[i].muv.configParams.l1TrigDSFactor;

		/*
		 std::bitset<8> enableMask(algoEnableMask[i]);
		 std::bitset<8> dwscMask(algoDwScMask[i]);
		 LOG_INFO("Downscale Factors CHOD: "
		 << algoDwScFactor[i][(uint) chodAlgorithmId]
		 << " RICH: "
		 << algoDwScFactor[i][(uint) richAlgorithmId]
		 << " KTAG: "
		 << algoDwScFactor[i][(uint) cedarAlgorithmId]
		 << " LAV: "
		 << algoDwScFactor[i][(uint) lavAlgorithmId]);
		 */
		CHODAlgo::initialize(i, l1Struct.l1Mask[i].chod);
//		RICHAlgo::initialize(l1Struct.l1Mask[i].rich);
		KtagAlgo::initialize(i, l1Struct.l1Mask[i].ktag);
		LAVAlgo::initialize(i, l1Struct.l1Mask[i].lav);
		MUV3Algo::initialize(i, l1Struct.l1Mask[i].muv);

		chodProcessID[i] = l1Struct.l1Mask[i].chod.configParams.l1TrigProcessID;
		richProcessID[i] = l1Struct.l1Mask[i].rich.configParams.l1TrigProcessID;
		cedarProcessID[i] =
				l1Struct.l1Mask[i].ktag.configParams.l1TrigProcessID;
		lavProcessID[i] = l1Struct.l1Mask[i].lav.configParams.l1TrigProcessID;
		ircsacProcessID[i] =
				l1Struct.l1Mask[i].ircsac.configParams.l1TrigProcessID;
		muvProcessID[i] = l1Struct.l1Mask[i].muv.configParams.l1TrigProcessID;

		chodEnableMask |= (l1Struct.l1Mask[i].chod.configParams.l1TrigEnable
				<< i);
		richEnableMask |= (l1Struct.l1Mask[i].rich.configParams.l1TrigEnable
				<< i);
		cedarEnableMask |= (l1Struct.l1Mask[i].ktag.configParams.l1TrigEnable
				<< i);
		lavEnableMask |=
				(l1Struct.l1Mask[i].lav.configParams.l1TrigEnable << i);
		ircsacEnableMask |= (l1Struct.l1Mask[i].ircsac.configParams.l1TrigEnable
				<< i);
		muvEnableMask |=
				(l1Struct.l1Mask[i].muv.configParams.l1TrigEnable << i);

		chodFlagMask |= (l1Struct.l1Mask[i].chod.configParams.l1TrigFlag << i);
		richFlagMask |= (l1Struct.l1Mask[i].rich.configParams.l1TrigFlag << i);
		cedarFlagMask |= (l1Struct.l1Mask[i].ktag.configParams.l1TrigFlag << i);
		lavFlagMask |= (l1Struct.l1Mask[i].lav.configParams.l1TrigFlag << i);
		ircsacFlagMask |= (l1Struct.l1Mask[i].ircsac.configParams.l1TrigFlag
				<< i);
		muvFlagMask |= (l1Struct.l1Mask[i].muv.configParams.l1TrigFlag << i);
	}
	/*
	 LOG_INFO("Enable Masks: CHOD " << chodEnableMask << " KTAG " << cedarEnableMask << " LAV " << lavEnableMask);

	 std::bitset<16> chodEnable(chodEnableMask);
	 std::bitset<16> richEnable(richEnableMask);
	 std::bitset<16> cedarEnable(cedarEnableMask);
	 std::bitset<16> lavEnable(lavEnableMask);
	 LOG_INFO("Enable Masks: CHOD " << chodEnable << " KTAG " << cedarEnable << " LAV " << lavEnable);

	 LOG_INFO("Flag Masks: CHOD " << chodFlagMask << " KTAG " << cedarFlagMask << " LAV " << lavFlagMask);

	 std::bitset<16> chodFlag(chodFlagMask);
	 std::bitset<16> richFlag(richFlagMask);
	 std::bitset<16> cedarFlag(cedarFlagMask);
	 std::bitset<16> lavFlag(lavFlagMask);
	 LOG_INFO("Flag Masks: CHOD " << chodFlag << " KTAG " << cedarFlag << " LAV " << lavFlag);
	 */
	int num = 0;
	for (int l0Mask : l0MaskIDs) {
		NumToMaskID[num] = l0Mask;
		MaskIDToNum[l0Mask] = num;
//		LOG_INFO("Initialization of Enabled Masks " << num << " " << l0Mask);
		num++;
	}
}

uint_fast8_t L1TriggerProcessor::compute(Event* const event) {

	using namespace l0;
	DecoderHandler decoder(event);
	event->readTriggerTypeWordAndFineTime();
	evtRefFineTime = event->getFinetime();

	uint_fast8_t l1Trigger = 0;
	uint_fast8_t l1GlobalFlagTrigger = 0;
	uint_fast8_t l1MaskFlagTrigger = 0;
	isL1Bypassed = 0;
	isAllL1AlgoDisable = 0;
	numberOfTriggeredL1Masks = 0;

	L1InputEvents_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO("L1Event number after adding 1 " << L1InputEvents_);
	L1InputEventsPerBurst_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO("L1Event number per Burst after adding 1 " << L1InputEventsPerBurst_);

//	LOG_INFO("Global FlagMode " << flagMode << " " << L1InputEvents_ << " " << autoFlagFactor);

	if (flagMode) {
		l1GlobalFlagTrigger = 1;
	} else if (autoFlagFactor && (L1InputEvents_ % autoFlagFactor == 0)) {
		l1GlobalFlagTrigger = 1;
	}
//	LOG_INFO("l1GlobalFlagTrigger " << (uint)l1GlobalFlagTrigger);
	/*
	 * Check if the event should bypass the processing
	 */
	if (event->isSpecialTriggerEvent()) {
		isL1Bypassed = 1;
		event->setRrequestZeroSuppressedCreamData(false);
		l1Trigger = ((uint) l1GlobalFlagTrigger << 7)
				| ((l1MaskFlagTrigger != 0) << 6) | (isL1Bypassed << 5)
				| (isAllL1AlgoDisable << 4) | (numberOfTriggeredL1Masks != 0);
		L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed);
//		L1TriggerProcessor::writeData(*l1Block);
		return l1Trigger;
	}
	if (event->isPulserGTKTriggerEvent() || bypassEvent()) {
		isL1Bypassed = 1;
		event->setRrequestZeroSuppressedCreamData(true);
		l1Trigger = ((uint) l1GlobalFlagTrigger << 7)
				| ((l1MaskFlagTrigger != 0) << 6) | (isL1Bypassed << 5)
				| (isAllL1AlgoDisable << 4) | (numberOfTriggeredL1Masks != 0);
		L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed);
//		L1TriggerProcessor::writeData(*l1Block);
		return l1Trigger;
	}

	/*
	 * Check if the event fulfills the reduction option
	 *
	 */
//	LOG_INFO("L1Reduction Factor " << reductionFactor);
//	LOG_INFO("Modulo " << L1InputEvents_ % reductionFactor);
	if (reductionFactor && (L1InputEvents_ % reductionFactor != 0))
		return 0;

	L1InputReducedEvents_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO("L1ReducedEvent number after adding 1 " << L1InputReducedEvents_);

	/*
	 * The event is ready to be processed
	 *
	 */
	l0TrigWord = event->getL0TriggerTypeWord();
//	LOG_INFO("l0TriggerWord " << std::hex << (uint) l0TrigWord << std::dec);
	l0TrigFlags = event->getTriggerFlags();
//	LOG_INFO("l0TriggerFlags " << std::hex << (uint) l0TrigFlags << std::dec);
	l0DataType = event->getTriggerDataType();
//	LOG_INFO("l0TriggerDataType " << std::hex << (uint) l0DataType << std::dec);

	/*
	 * Store the global event timestamp taken from the reverence detector
	 */
	l0::MEPFragment* tsFragment = event->getL0SubeventBySourceIDNum(
			SourceIDManager::TS_SOURCEID_NUM)->getFragment(0);
	event->setTimestamp(tsFragment->getTimestamp());

	isL0PhysicsTrigger = 0;
	isL0PeriodicTrigger = 0;
	isL0ControlTrigger = 0;
	isReducedEvent = 0;
	isAlgoEnableForAllL0Masks = 0;
	chodTrigger = 0;
	richTrigger = 0;
	cedarTrigger = 0;
	lavTrigger = 0;
	ircsacTrigger = 0;
	muvTrigger = 0;
	uint_fast8_t l1TriggerTmp;
	uint_fast8_t l1FlagTrigger;
	uint watchingWhileLoops;

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

	if (isL0PhysicsTrigger) {
		for (int i = 0; i != 16; i++) {
			l1TriggerWords[i] = 0;
			if (l0TrigFlags & (1 << i)) {
				L1InputReducedEventsPerL0Mask_[i].fetch_add(1,
						std::memory_order_relaxed);
				l1ProcessID = 0;
				watchingWhileLoops = 0;
				l1TriggerTmp = 0;
				l1FlagTrigger = 0;

//				l1FlagTrigger = (algoEnableMask[i] & algoFlagMask[i]);
				if (numberOfEnabledAndFlaggedAlgos[i])
					l1FlagTrigger = 1;
//				LOG_INFO("Reduction factor " << maskReductionFactor[i] << " Modulo (maskReductionFactor) " << L1InputReducedEventsPerL0Mask_[i] % maskReductionFactor[i]);
				if (maskReductionFactor[i]
						&& (L1InputReducedEventsPerL0Mask_[i]
								% maskReductionFactor[i] != 0))
					isReducedEvent = 1;
				else
					isReducedEvent = 0;

//				LOG_INFO("isReducedEvent " << isReducedEvent);
//				LOG_INFO("i " << i << " processID " << l1ProcessID << " nEnAlgos " << numberOfEnabledAlgos[i] << " l1TriggerTemp " << (uint) l1TriggerTmp << " nEnableFlagAlgos " << numberOfEnabledAndFlaggedAlgos[i] << " l1FlagTrig " << (uint) l1FlagTrigger);
				if (!numberOfEnabledAlgos[i])
					isAllL1AlgoDisable = 1;

				while ((watchingWhileLoops != 10) && !isReducedEvent
						&& l1ProcessID != numberOfEnabledAlgos[i]) {

					if ((chodEnableMask & (1 << i))
							&& (chodProcessID[i] == l1ProcessID)
							&& SourceIDManager::isChodActive()) {
						if (!CHODAlgo::isAlgoProcessed()) {
							chodTrigger = CHODAlgo::processCHODTrigger(i,
									decoder, l1Info_);
//					if (chodTrigger != 0) {
//						L1Downscaling::processAlgorithm(chodAlgorithmId);
//					}
						}
						l1ProcessID++;
						l1TriggerTmp |= (chodTrigger << (uint) chodAlgorithmId);
//						printf("L1TriggerProcessor.cpp: chodTrigger %d\n",
//								chodTrigger);
					}
					if ((chodEnableMask & l0TrigFlags) == l0TrigFlags)
						isAlgoEnableForAllL0Masks = 1;
					if (!(l1TriggerTmp & algoEnableMask[i]) && l1ProcessID
							&& !((chodEnableMask & chodFlagMask) & (1 << i))) {
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
					if ((cedarEnableMask & (1 << i))
							&& cedarProcessID[i] == l1ProcessID
							&& SourceIDManager::isCedarActive()) {
						if (!KtagAlgo::isAlgoProcessed()) {
							cedarTrigger = KtagAlgo::processKtagTrigger(i,
									decoder, l1Info_);
//					if (cedarTrigger != 0) {
//						L1Downscaling::processAlgorithm(cedarAlgorithmId);
//					}
						}
						l1ProcessID++;
						l1TriggerTmp |=
								(cedarTrigger << (uint) cedarAlgorithmId);
//						printf("L1TriggerProcessor.cpp: cedarTrigger %d\n",
//								cedarTrigger);
					}
					if ((cedarEnableMask & l0TrigFlags) == l0TrigFlags)
						isAlgoEnableForAllL0Masks = 1;
					if (!(l1TriggerTmp & algoEnableMask[i]) && l1ProcessID
							&& !((cedarEnableMask & cedarFlagMask) & (1 << i))) {
						watchingWhileLoops++;
						break;
					}

					if ((lavEnableMask & (1 << i))
							&& lavProcessID[i] == l1ProcessID
							&& SourceIDManager::isLavActive()) {
						if (!LAVAlgo::isAlgoProcessed()) {
							lavTrigger = LAVAlgo::processLAVTrigger(i, decoder,
									l1Info_);
//					if (lavTrigger != 0) {
//						L1Downscaling::processAlgorithm(lavAlgorithmId);
//					}
						}
						l1ProcessID++;
						l1TriggerTmp |= (lavTrigger << (uint) lavAlgorithmId);
//						printf("L1TriggerProcessor.cpp: lavTrigger %d\n",
//								lavTrigger);
					}
					if ((lavEnableMask & l0TrigFlags) == l0TrigFlags)
						isAlgoEnableForAllL0Masks = 1;
					if (!(l1TriggerTmp & algoEnableMask[i]) && l1ProcessID
							&& !((lavEnableMask & lavFlagMask) & (1 << i))) {
						watchingWhileLoops++;
						break;
					}

					if ((muvEnableMask & (1 << i))
							&& muvProcessID[i] == l1ProcessID
							&& SourceIDManager::isMUV3Active()) {
						if (!MUV3Algo::isAlgoProcessed()) {
							muvTrigger = MUV3Algo::processMUV3Trigger0(i,
									decoder, l1Info_);
//							if (muvTrigger != 0) {
//								L1Downscaling::processAlgorithm(muvAlgorithmId);
//							}
						}
						l1ProcessID++;
						l1TriggerTmp |= (muvTrigger << (uint) muvAlgorithmId);
//						printf("L1TriggerProcessor.cpp: muvTrigger %d\n",
//								muvTrigger);
					}
					if ((muvEnableMask & l0TrigFlags) == l0TrigFlags)
						isAlgoEnableForAllL0Masks = 1;
					if (!(l1TriggerTmp & algoEnableMask[i]) && l1ProcessID
							&& !((muvEnableMask & muvFlagMask) & (1 << i))) {
						watchingWhileLoops++;
						break;
					}

					watchingWhileLoops++;
					if (!l1ProcessID && watchingWhileLoops)
						LOG_ERROR(
								"No Algorithms are being processed within the While Loop !");
				}

				if ((watchingWhileLoops == 10)
						&& (l1ProcessID != numberOfEnabledAlgos[i]))
					LOG_ERROR(
							"TimeOut while loop: check why enabled algos do not get processed !");

//				LOG_INFO("watchingWhileLoops " << watchingWhileLoops);
//				LOG_INFO("isReducedEvent " << isReducedEvent);
//				LOG_INFO("l1ProcessID " << l1ProcessID);
//				LOG_INFO("numberOfEnabledAlgos " << numberOfEnabledAlgos[i]);

//				printf("L1TriggerProcessor.cpp: l1Trigger (!!TMP!!) %x\n",l1TriggerTmp);
//				LOG_INFO("isReducedEvent " << isReducedEvent << " EnableMask " << algoEnableMask[i] << " trigTmp & EnableMask " << (l1TriggerTmp & algoEnableMask[i]) << " l1FlagTrig " << (uint) l1FlagTrigger);
				if (!isReducedEvent
						&& (!algoEnableMask[i]
								|| ((l1TriggerTmp & algoEnableMask[i])
										== algoEnableMask[i])
								|| (l1TriggerTmp
										& (algoEnableMask[i]
												& (0xFF - algoFlagMask[i]))))) {
					L1AcceptedEventsPerL0Mask_[i].fetch_add(1,
							std::memory_order_relaxed);

//					LOG_INFO("L1 Accepted Event Per L0 mask " << i << " number after adding 1 " << L1AcceptedEventsPerL0Mask_[i]);
//					LOG_INFO("TrigTmp " << (uint) l1TriggerTmp << " dwscMask " << (uint) algoDwScMask[i]);
//					LOG_INFO("downscale Factor " << algoDwScFactor[i][__builtin_ctz( (uint) algoDwScMask[i])]);
//					LOG_INFO("Modulo " << L1AcceptedEventsPerL0Mask_[i] % algoDwScFactor[i][__builtin_ctz( (uint) algoDwScMask[i])]);

					l1MaskFlagTrigger += l1FlagTrigger;
//					LOG_INFO("l1MaskFlagTrigger " << (uint)l1MaskFlagTrigger);

					if ((l1TriggerTmp & algoDwScMask[i])
							&& (++(eventCountersByL0MaskByAlgoID_[i][__builtin_ctz(
									(uint) algoDwScMask[i])])
									% algoDwScFactor[i][__builtin_ctz(
											(uint) algoDwScMask[i])] != 0)) {

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
						l1TriggerWords[i] = ((l1TriggerTmp & algoEnableMask[i])
								== algoEnableMask[i]);
				} else
					l1TriggerWords[i] = 0;
//				printf("L1TriggerProcessor.cpp: l1Trigger %x\n",
//						l1TriggerWords[i]);
			}
			if (__builtin_popcount((uint) l1TriggerWords[i]))
				numberOfTriggeredL1Masks++;
		}

//		printf("Summary of Triggered Masks: %d\n", numberOfTriggeredL1Masks);
//		for (int i = 0; i != 16; i++) printf("Summary of Trigger Words: l1Trigger %x\n",l1TriggerWords[i]);

		if (l1Info_->isL1CHODProcessed())
			l1Info_->resetL1CHODProcessed();
//	LOG_INFO("Reset L1CHODProcessed boolean " << l1Info_->isL1CHODProcessed());

		if (CHODAlgo::isAlgoProcessed())
			CHODAlgo::resetAlgoProcessed();
		if (KtagAlgo::isAlgoProcessed())
			KtagAlgo::resetAlgoProcessed();
		if (LAVAlgo::isAlgoProcessed())
			LAVAlgo::resetAlgoProcessed();
		if (MUV3Algo::isAlgoProcessed())
			MUV3Algo::resetAlgoProcessed();
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

	l1Trigger = ((uint) l1GlobalFlagTrigger << 7)
			| ((l1MaskFlagTrigger != 0) << 6) | (isL1Bypassed << 5)
			| (isAllL1AlgoDisable << 4) | (numberOfTriggeredL1Masks != 0);

	/*
	 if (isL0ControlTrigger) {
	 l1Trigger = ((uint) l1GlobalFlagTrigger << 7) | (1 << 4)
	 | (numberOfTriggeredL1Masks != 0);
	 event->setRrequestZeroSuppressedCreamData(true);
	 L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed);
	 printf("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger %x\n",
	 l1Trigger);
	 L1TriggerProcessor::writeData(*l1Block);
	 return l1Trigger;
	 }

	 if (isL0PeriodicTrigger) {
	 l1Trigger = ((uint) l1GlobalFlagTrigger << 7) | (1 << 5)
	 | (numberOfTriggeredL1Masks != 0);
	 event->setRrequestZeroSuppressedCreamData(true);
	 L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed);
	 printf("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger %x\n",
	 l1Trigger);
	 L1TriggerProcessor::writeData(*l1Block);
	 return l1Trigger;
	 }

	 if (isAllL1AlgoDisable) {
	 l1Trigger = ((uint) l1GlobalFlagTrigger << 7) | (1 << 6)
	 | (numberOfTriggeredL1Masks != 0);
	 event->setRrequestZeroSuppressedCreamData(true);
	 L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed);
	 printf("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger %x\n",
	 l1Trigger);
	 L1TriggerProcessor::writeData(*l1Block);
	 return l1Trigger;
	 }
	 */
//	LOG_INFO((uint) l1GlobalFlagTrigger << " " << isDownscaledAndFlaggedEvent << " " << ((uint) l1GlobalFlagTrigger && !isDownscaledAndFlaggedEvent));
//	l1Trigger = ((uint) l1GlobalFlagTrigger << 7) | (numberOfTriggeredL1Masks != 0);
//	l1Trigger = (((uint) l1GlobalFlagTrigger || !isDownscaledAndFlaggedEvent) << 7) | l1TriggerWords[2];
//	l1Trigger = (l1GlobalFlagTrigger << 7) | (lavTrigger << 3) | (cedarTrigger << 2) | (richTrigger << 1) | chodTrigger;
//	l1Trigger = (cedarTrigger != 0 && chodTrigger != 0);
	if (l1Trigger != 0) {
		/*
		 * TODO: Pass this counter to Nicolas to print number of events before downscaling
		 */
		L1AcceptedEvents_.fetch_add(1, std::memory_order_relaxed);
//		LOG_INFO("L1 Accepted Event number after adding 1 " << L1AcceptedEvents_);

//Global L1 downscaling
//		LOG_INFO("L1Downscale Factor " << downscaleFactor);
//		LOG_INFO("Modulo " << L1AcceptedEvents_ % downscaleFactor);
		if (downscaleFactor && (L1AcceptedEvents_ % downscaleFactor != 0))
			return 0;
		/*
		 * Decision whether or not to request zero suppressed data from the creams
		 */
		event->setRrequestZeroSuppressedCreamData(true);
		event->setProcessingID(0); // 0 indicates raw data as collected from the detector
//		L1TriggerProcessor::writeData(*l1Block);
	}

//	std::bitset<8> l1TrgWrd(l1Trigger);
//	printf("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger %8x\n", l1Trigger);
//	LOG_INFO("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger " << l1TrgWrd);
	L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed); // The second 8 bits are the L1 trigger type word
	return l1Trigger;
}

void L1TriggerProcessor::writeData(L1Block &l1Block) {

	(l1Block.l1Global).PCfarmSoftwareID = 0; //Fill with correct value!!!
	(l1Block.l1Global).refFineTime = evtRefFineTime;
	(l1Block.l1Global).l0TriggerType = l0TrigWord;
	(l1Block.l1Global).l0DataType = l0DataType;
	(l1Block.l1Global).l0TriggerFlags = l0TrigFlags;
	(l1Block.l1Global).l1BypassProbability = bypassProbability;
	(l1Block.l1Global).l1FlagMode = flagMode;
	(l1Block.l1Global).refTimeSourceID = referenceTimeSourceID;
	(l1Block.l1Global).l1AutoFlagFactor = autoFlagFactor;
	(l1Block.l1Global).l1ReductionFactor = reductionFactor;
	(l1Block.l1Global).l1DownscaleFactor = downscaleFactor;
//	LOG_INFO(
//			"refFineTime " << (uint)(l1Block.l1Global).refFineTime << " l0TriggerType " << (uint)(l1Block.l1Global).l0TriggerType << " l0DataType " << (uint)(l1Block.l1Global).l0DataType);
//	LOG_INFO(
//			"triggerFlags " << (uint)(l1Block.l1Global).l0TriggerFlags << " bypassProb " << (uint)(l1Block.l1Global).l1BypassProbability);
//	LOG_INFO(
//			"flagMode " << (uint)(l1Block.l1Global).l1FlagMode << " refTimeSourceID "<< (uint)(l1Block.l1Global).refTimeSourceID);
//	LOG_INFO(
//			"autoFlagFactor " << (uint)(l1Block.l1Global).l1AutoFlagFactor << " reductionFactor "<< (uint)(l1Block.l1Global).l1ReductionFactor);
//	LOG_INFO("downscaleFactor " << (uint)(l1Block.l1Global).l1DownscaleFactor);

	int numToMaskID;
	for (int iNum = 0; iNum < numberOfEnabledL0Masks; iNum++) {
		if (NumToMaskID[iNum] == -1)
			LOG_ERROR("ERROR! Wrong association of mask ID!");
		else
			numToMaskID = NumToMaskID[iNum];
		(l1Block.l1Mask[iNum]).maskID = numToMaskID;
		(l1Block.l1Mask[iNum]).triggerWord = l1TriggerWords[numToMaskID];
		(l1Block.l1Mask[iNum]).numberOfEnabledAlgos =
				numberOfEnabledAlgos[numToMaskID];
		(l1Block.l1Mask[iNum]).numberOfFlaggedAlgos =
				numberOfFlaggedAlgos[numToMaskID];
		(l1Block.l1Mask[iNum]).reductionFactor =
				maskReductionFactor[numToMaskID];
		(l1Block.l1Mask[iNum]).algoEnableMask = algoEnableMask[numToMaskID];
		(l1Block.l1Mask[iNum]).algoFlagMask = algoFlagMask[numToMaskID];
		(l1Block.l1Mask[iNum]).algoLogicMask = algoLogicMask[numToMaskID];
		(l1Block.l1Mask[iNum]).algoDwScMask = algoDwScMask[numToMaskID];

//		LOG_INFO(
//				"maskID " << (uint)(l1Block.l1Mask[iNum]).maskID << " triggerWord " << (uint)(l1Block.l1Mask[iNum]).triggerWord);
//		LOG_INFO(
//				"numberOfEnabledAlgos " << (uint)(l1Block.l1Mask[iNum]).numberOfEnabledAlgos << " numberOfFlaggedAlgos " << (uint)(l1Block.l1Mask[iNum]).numberOfFlaggedAlgos);
//		LOG_INFO(
//				"reductionFactor " << (uint)(l1Block.l1Mask[iNum]).reductionFactor << " enableMask "<< (uint)(l1Block.l1Mask[iNum]).algoEnableMask);
//		LOG_INFO(
//				"flagMask " << (uint)(l1Block.l1Mask[iNum]).algoFlagMask << " logicMask "<< (uint)(l1Block.l1Mask[iNum]).algoLogicMask);
//		LOG_INFO("downscaleMask " << (uint)(l1Block.l1Mask[iNum]).algoDwScMask);

		(l1Block.l1Mask[iNum]).l1Algo[chodAlgorithmId].l1AlgoDSFactor =
				algoDwScFactor[numToMaskID][chodAlgorithmId];
		(l1Block.l1Mask[iNum]).l1Algo[cedarAlgorithmId].l1AlgoDSFactor =
				algoDwScFactor[numToMaskID][cedarAlgorithmId];
		(l1Block.l1Mask[iNum]).l1Algo[lavAlgorithmId].l1AlgoDSFactor =
				algoDwScFactor[numToMaskID][lavAlgorithmId];
		(l1Block.l1Mask[iNum]).l1Algo[ircsacAlgorithmId].l1AlgoDSFactor =
				algoDwScFactor[numToMaskID][ircsacAlgorithmId];
		(l1Block.l1Mask[iNum]).l1Algo[muvAlgorithmId].l1AlgoDSFactor =
				algoDwScFactor[numToMaskID][muvAlgorithmId];

//		LOG_INFO(
//				"chodAlgo " << (uint)chodAlgorithmId << " DSFactor " << (uint)(l1Block.l1Mask[iNum]).l1Algo[chodAlgorithmId].l1AlgoDSFactor);
//		LOG_INFO(
//				"cedarAlgo " << (uint)cedarAlgorithmId << " DSFactor " << (uint)(l1Block.l1Mask[iNum]).l1Algo[cedarAlgorithmId].l1AlgoDSFactor);
//		LOG_INFO(
//				"lavAlgo " << (uint)lavAlgorithmId << " DSFactor " << (uint)(l1Block.l1Mask[iNum]).l1Algo[lavAlgorithmId].l1AlgoDSFactor);

		(l1Block.l1Mask[iNum]).l1Algo[chodAlgorithmId].l1AlgoProcessID =
				chodProcessID[numToMaskID];
		(l1Block.l1Mask[iNum]).l1Algo[cedarAlgorithmId].l1AlgoProcessID =
				cedarProcessID[numToMaskID];
		(l1Block.l1Mask[iNum]).l1Algo[lavAlgorithmId].l1AlgoProcessID =
				lavProcessID[numToMaskID];
		(l1Block.l1Mask[iNum]).l1Algo[ircsacAlgorithmId].l1AlgoProcessID =
				ircsacProcessID[numToMaskID];
		(l1Block.l1Mask[iNum]).l1Algo[muvAlgorithmId].l1AlgoProcessID =
				muvProcessID[numToMaskID];
//		LOG_INFO(
//				"chodAlgo " << (uint)chodAlgorithmId << " ProcessID " << (uint)(l1Block.l1Mask[iNum]).l1Algo[chodAlgorithmId].l1AlgoProcessID);
//		LOG_INFO(
//				"cedarAlgo " << (uint)cedarAlgorithmId << " ProcessID " << (uint)(l1Block.l1Mask[iNum]).l1Algo[cedarAlgorithmId].l1AlgoProcessID);
//		LOG_INFO(
//				"lavAlgo " << (uint)lavAlgorithmId << " ProcessID " << (uint)(l1Block.l1Mask[iNum]).l1Algo[lavAlgorithmId].l1AlgoProcessID);
	}
}
}
/* namespace na62 */
