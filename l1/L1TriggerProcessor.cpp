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
uint16_t L1TriggerProcessor::algoDwScFactor[16][10];
int L1TriggerProcessor::algoProcessID[16][10];

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

uint L1TriggerProcessor::chodAlgorithmId;
uint L1TriggerProcessor::richAlgorithmId;
uint L1TriggerProcessor::cedarAlgorithmId;
uint L1TriggerProcessor::lavAlgorithmId;
uint L1TriggerProcessor::ircsacAlgorithmId;
uint L1TriggerProcessor::muvAlgorithmId;

uint_fast8_t L1TriggerProcessor::l0TrigWord = 0;
uint_fast8_t L1TriggerProcessor::l0DataType = 0;
uint_fast16_t L1TriggerProcessor::l0TrigFlags = 0;
uint_fast8_t L1TriggerProcessor::l1Trigger = 0;
uint_fast8_t L1TriggerProcessor::l1GlobalFlagTrigger = 0;
uint_fast8_t L1TriggerProcessor::l1MaskFlagTrigger = 0;

uint_fast8_t L1TriggerProcessor::chodTrigger = 0;
uint_fast8_t L1TriggerProcessor::richTrigger = 0;
uint_fast8_t L1TriggerProcessor::cedarTrigger = 0;
uint_fast8_t L1TriggerProcessor::lavTrigger = 0;
uint_fast8_t L1TriggerProcessor::ircsacTrigger = 0;
uint_fast8_t L1TriggerProcessor::muvTrigger = 0;
uint_fast8_t L1TriggerProcessor::l1TriggerWords[16];

uint L1TriggerProcessor::MaskIDToNum[16];
uint L1TriggerProcessor::NumToMaskID[16];
uint L1TriggerProcessor::AlgoIDToNum[16][10];
uint L1TriggerProcessor::NumToAlgoID[16][10];

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
bool L1TriggerProcessor::isL1WhileTimeout = 0;
uint_fast8_t L1TriggerProcessor::numberOfEnabledL0Masks = 0;

std::vector<int> L1TriggerProcessor::l0MaskIDs;
uint_fast32_t L1TriggerProcessor::L1DataPacketSize = 0;
char * L1TriggerProcessor::buffer(nullptr);

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

void L1TriggerProcessor::clear() {
	l1Trigger = 0;
	l1GlobalFlagTrigger = 0;
	l1MaskFlagTrigger = 0;
	isL1Bypassed = 0;
	isAllL1AlgoDisable = 0;
	isL1WhileTimeout = 0;
	numberOfTriggeredL1Masks = 0;
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
}

void L1TriggerProcessor::initialize(l1Struct &l1Struct) {
	// Seed for rand()
	srand(time(NULL));

	l0MaskIDs = TriggerOptions::GetIntList(OPTION_ACTIVE_L0_MASKS);
	numberOfEnabledL0Masks = l0MaskIDs.size();
//	LOG_INFO("numberOfEnabledL0Masks " << (uint)numberOfEnabledL0Masks);

	for (int i = 0; i != 0xFF + 1; i++) {
		L1Triggers_[i] = 0;
	}
	eventCountersByL0MaskByAlgoID_ = new std::atomic<uint64_t>*[16];

	for (int i = 0; i != 16; ++i) {
		eventCountersByL0MaskByAlgoID_[i] = new std::atomic<uint64_t>[10] { };
		L1AcceptedEventsPerL0Mask_[i] = 0;
		L1InputReducedEventsPerL0Mask_[i] = 0;
		numberOfEnabledAlgos[i] = 0;
		numberOfFlaggedAlgos[i] = 0;
		numberOfEnabledAndFlaggedAlgos[i] = 0;
		maskReductionFactor[i] = 0;
		algoEnableMask[i] = 0;
		algoFlagMask[i] = 0;
		algoLogicMask[i] = 0;
		algoDwScMask[i] = 0;
		NumToMaskID[i] = -1;
		MaskIDToNum[i] = -1;
		l1TriggerWords[i] = 0;
		for (int j = 0; j != 10; ++j) {
			eventCountersByL0MaskByAlgoID_[i][j] = 0;
			algoDwScFactor[i][j] = 1;
			algoProcessID[i][j] = -1;
			NumToAlgoID[i][j] = -1;
			AlgoIDToNum[i][j] = -1;
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
//		LOG_INFO("L0 Mask " << i << " number of Enabled algo " << numberOfEnabledAlgos[i] << " Flagged " << numberOfFlaggedAlgos[i] << " EnabledAndFlagged " << numberOfEnabledAndFlaggedAlgos[i]);

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

		algoProcessID[i][(uint) chodAlgorithmId] =
				l1Struct.l1Mask[i].chod.configParams.l1TrigProcessID;
		algoProcessID[i][(uint) richAlgorithmId] =
				l1Struct.l1Mask[i].rich.configParams.l1TrigProcessID;
		algoProcessID[i][(uint) cedarAlgorithmId] =
				l1Struct.l1Mask[i].ktag.configParams.l1TrigProcessID;
		algoProcessID[i][(uint) lavAlgorithmId] =
				l1Struct.l1Mask[i].lav.configParams.l1TrigProcessID;
		algoProcessID[i][(uint) ircsacAlgorithmId] =
				l1Struct.l1Mask[i].ircsac.configParams.l1TrigProcessID;
		algoProcessID[i][(uint) muvAlgorithmId] =
				l1Struct.l1Mask[i].muv.configParams.l1TrigProcessID;

//		LOG_INFO("L0Mask " << i << " ProcessID CHOD: " << algoProcessID[i][(uint) chodAlgorithmId] << " RICH: " << algoProcessID[i][(uint) richAlgorithmId] << " KTAG: " << algoProcessID[i][(uint) cedarAlgorithmId] << " LAV: " << algoProcessID[i][(uint) lavAlgorithmId] << " IRCSAC: " << algoProcessID[i][(uint) ircsacAlgorithmId] << " MUV3: " << algoProcessID[i][(uint) muvAlgorithmId]);

//		std::bitset<8> enableMask(algoEnableMask[i]);
//		std::bitset<8> flagMask(algoFlagMask[i]);
//		std::bitset<8> logicMask(algoLogicMask[i]);
//		std::bitset<8> dwscMask(algoDwScMask[i]);
//		LOG_INFO("algoEnableMask " << i << " " << enableMask);
//		LOG_INFO("algoFlagMask " << i << " " << flagMask);
//		LOG_INFO("algoLogicMask " << i << " " << logicMask);
//		LOG_INFO("algoDwScMask " << i << " " << dwscMask);

//		LOG_INFO("Downscale Factors CHOD: " << algoDwScFactor[i][(uint) chodAlgorithmId] << " RICH: " << algoDwScFactor[i][(uint) richAlgorithmId] << " KTAG: " << algoDwScFactor[i][(uint) cedarAlgorithmId] << " LAV: " << algoDwScFactor[i][(uint) lavAlgorithmId]);

		CHODAlgo::initialize(i, l1Struct.l1Mask[i].chod);
//		RICHAlgo::initialize(l1Struct.l1Mask[i].rich);
		KtagAlgo::initialize(i, l1Struct.l1Mask[i].ktag);
		LAVAlgo::initialize(i, l1Struct.l1Mask[i].lav);
		MUV3Algo::initialize(i, l1Struct.l1Mask[i].muv);

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
	L1DataPacketSize = sizeof(L1Global);

	int num = 0;
	int algoNum = 0;
	for (int l0Mask : l0MaskIDs) {
		NumToMaskID[num] = l0Mask;
		MaskIDToNum[l0Mask] = num;
//		LOG_INFO("Initialization of Enabled Masks " << MaskIDToNum[l0Mask] << " " << NumToMaskID[num]);
		L1DataPacketSize += (sizeof(L1Mask)
				+ numberOfEnabledAlgos[l0Mask] * sizeof(L1Algo));
		for (int i = 0; i != 16; i++) {
			if (algoEnableMask[l0Mask] & (1 << i)) {
				NumToAlgoID[num][algoNum] = i;
				AlgoIDToNum[num][i] = algoNum;
//				LOG_INFO("Initialization of Enabled Algos " << AlgoIDToNum[num][i] << " " << NumToAlgoID[num][algoNum]);
				algoNum++;
			}
		}
		num++;
		algoNum = 0;
	}

//	LOG_INFO("Size of L1 Data Packet " << (uint)L1DataPacketSize);
	buffer = new char[(uint) L1DataPacketSize]();
}

uint_fast8_t L1TriggerProcessor::compute(Event* const event) {

	using namespace l0;
	DecoderHandler decoder(event);
	event->readTriggerTypeWordAndFineTime();
	evtRefFineTime = event->getFinetime();

	L1TriggerProcessor::clear();

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
//		printf("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger %8x\n",l1Trigger);
		L1TriggerProcessor::writeL1Data(event);
//		L1TriggerProcessor::readL1Data(event);
		return l1Trigger;
	}
	if (event->isPulserGTKTriggerEvent() || bypassEvent()) {
		isL1Bypassed = 1;
		event->setRrequestZeroSuppressedCreamData(true);
		l1Trigger = ((uint) l1GlobalFlagTrigger << 7)
				| ((l1MaskFlagTrigger != 0) << 6) | (isL1Bypassed << 5)
				| (isAllL1AlgoDisable << 4) | (numberOfTriggeredL1Masks != 0);
		L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed);
//		printf("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger %8x\n",l1Trigger);
		L1TriggerProcessor::writeL1Data(event);
//		L1TriggerProcessor::readL1Data(event);
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

	KtagAlgo::clear();
	CHODAlgo::clear();
	LAVAlgo::clear();
	MUV3Algo::clear();
	l1Info_->resetL1CHODProcessed();

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
							&& (algoProcessID[i][chodAlgorithmId] == l1ProcessID)
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
							&& algoProcessID[i][cedarAlgorithmId] == l1ProcessID
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
							&& algoProcessID[i][lavAlgorithmId] == l1ProcessID
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
							&& algoProcessID[i][muvAlgorithmId] == l1ProcessID
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
						&& (l1ProcessID != numberOfEnabledAlgos[i])) {
					isL1WhileTimeout++;
					LOG_ERROR(
							"TimeOut while loop: check why enabled algos do not get processed !");
				}

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
		L1TriggerProcessor::writeL1Data(event);
//		L1TriggerProcessor::readL1Data(event);

	}

//	std::bitset<8> l1TrgWrd(l1Trigger);
//	printf("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger %8x\n", l1Trigger);
//	LOG_INFO("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger " << l1TrgWrd);
	L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed); // The second 8 bits are the L1 trigger type word
	return l1Trigger;
}

void L1TriggerProcessor::readL1Data(Event* const event) {
	const l0::MEPFragment* const L1TPEvent =
			event->getL1ResultSubevent()->getFragment(0);
	char* pload = (char*) L1TPEvent->getPayload();
	uint_fast16_t length = (uint_fast16_t) L1TPEvent->getPayloadLength();
	if ((uint) length != (uint) L1DataPacketSize)
		LOG_ERROR(
				"length (in bytes) " << (uint)length << " is different from l1DataPacketsSize " << (uint)L1DataPacketSize);
	uint32_t dataWord;
	for (uint i = 0; i != (uint) L1DataPacketSize / 4; i++) {
		L1Data* dataPacket = (L1Data*) pload + i;
		std::bitset<32> data((uint) dataPacket->dataWords);
		LOG_INFO("L1 Data " << data);
	}
}
void L1TriggerProcessor::writeL1Data(Event* const event) {

	uint nBlockHeaderWords = 0;
	uint_fast8_t refTimeSourceID = 0;
	uint_fast8_t refTimeSourceID_tmp = 0;
	memset(reinterpret_cast<void*>(buffer), 0, (uint) L1DataPacketSize);

	L1Global* globalPacket = reinterpret_cast<L1Global*>(buffer);

	globalPacket->globalPacketLength = sizeof(L1Global);
	globalPacket->refFineTime = evtRefFineTime;
	globalPacket->refTimeSourceID = referenceTimeSourceID;
	globalPacket->flagMode = flagMode;
	globalPacket->format = 0;
	globalPacket->downscaleFactor = downscaleFactor;
	globalPacket->reductionFactor = reductionFactor;
	globalPacket->numberOfEnabledMasks = numberOfEnabledL0Masks;
	globalPacket->bypassProbability = bypassProbability;
	globalPacket->autoFlagFactor = autoFlagFactor;

	nBlockHeaderWords += sizeof(L1Global); //3 32-bit header words for global part

	/*
	 LOG_INFO("L0 Trigger Flags " << (uint)l0TrigFlags);
	 LOG_INFO(
	 "Global Packet Length " << (uint)globalPacket->globalPacketLength << " refFineTime " << (uint)globalPacket->refFineTime << " RefTimeSourceID "<< (uint)globalPacket->refTimeSourceID << " flagMode " << (uint)globalPacket->flagMode << " format " << (uint)globalPacket->format);
	 LOG_INFO(
	 "DownscaleFactor " << (uint)globalPacket->downscaleFactor << " reductionFactor "<< (uint)globalPacket->reductionFactor);
	 LOG_INFO(
	 "Number of Enabled Masks " << (uint)globalPacket->numberOfEnabledMasks << " bypassProb " << (uint)globalPacket->bypassProbability << " autoFlagFactor " << (uint)globalPacket->autoFlagFactor);
	 */

	uint nMaskWords = 0;
	int numToMaskID;
	int numToAlgoID;
	for (int iNum = 0; iNum < numberOfEnabledL0Masks; iNum++) {
		if (NumToMaskID[iNum] == -1)
			LOG_ERROR("ERROR! Wrong association of mask ID!");
		else
			numToMaskID = NumToMaskID[iNum];

		L1Mask* maskPacket = (L1Mask*) (buffer + nBlockHeaderWords + nMaskWords);

		maskPacket->numberOfEnabledAlgos = numberOfEnabledAlgos[numToMaskID];
		maskPacket->triggerWord = l1TriggerWords[numToMaskID];
		maskPacket->maskID = numToMaskID;
		if (numberOfEnabledAndFlaggedAlgos[numToMaskID])
			maskPacket->flags |= (1 << 6);
		if (!numberOfEnabledAlgos[numToMaskID])
			maskPacket->flags |= (1 << 4);
		if (isL1WhileTimeout)
			maskPacket->flags |= (1 << 2);
		maskPacket->reductionFactor = maskReductionFactor[numToMaskID];
		maskPacket->reserved = 0;

		nMaskWords += sizeof(L1Mask); //2 32-bit header words for each mask

		/*
		 LOG_INFO(
		 "maskID " << (uint) maskPacket->maskID << " triggerWord " << (uint) maskPacket->triggerWord);
		 LOG_INFO(
		 "numberOfEnabledAlgos " << (uint)maskPacket->numberOfEnabledAlgos);
		 LOG_INFO("reductionFactor " << (uint)maskPacket->reductionFactor);

		 std::bitset<8> maskFlag((uint) maskPacket->flags);
		 LOG_INFO("Flags " << (uint)maskPacket->flags << " " << maskFlag);
		 */

		for (int iAlgoNum = 0; iAlgoNum < numberOfEnabledAlgos[numToMaskID];
				iAlgoNum++) {
			if (NumToAlgoID[iNum][iAlgoNum] == -1)
				LOG_ERROR("ERROR! Wrong association of algo ID!");
			else
				numToAlgoID = NumToAlgoID[iNum][iAlgoNum];

			L1Algo* algoPacket = (L1Algo*) (buffer + nBlockHeaderWords
					+ nMaskWords);

			if (l0TrigFlags & (1 << numToMaskID)) {

				algoPacket->processID = algoProcessID[numToMaskID][numToAlgoID];
				algoPacket->algoID = numToAlgoID;
				algoPacket->downscaleFactor =
						algoDwScFactor[numToMaskID][numToAlgoID];
				/*
				 LOG_INFO(
				 "Enable " << ((algoEnableMask[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
				 LOG_INFO(
				 "Flag " << ((algoFlagMask[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
				 LOG_INFO(
				 "Logic " << ((algoLogicMask[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
				 LOG_INFO(
				 "DwSc " << ((algoDwScMask[numToMaskID] & (1 << numToAlgoID)) >> numToAlgoID));
				 */

				algoPacket->algoFlags = (((algoEnableMask[numToMaskID]
						& (1 << numToAlgoID)) >> numToAlgoID) << 6)
						| (((algoFlagMask[numToMaskID] & (1 << numToAlgoID))
								>> numToAlgoID) << 4)
						| (((algoLogicMask[numToMaskID] & (1 << numToAlgoID))
								>> numToAlgoID) << 2)
						| (((algoDwScMask[numToMaskID] & (1 << numToAlgoID))
								>> numToAlgoID));

				writeAlgoPacket(numToAlgoID, algoPacket, numToMaskID);

				refTimeSourceID_tmp = (algoPacket->qualityFlags & 0x3);
				if (refTimeSourceID != refTimeSourceID_tmp)
					refTimeSourceID = refTimeSourceID_tmp;
			} else {
				algoPacket->numberOfWords = (sizeof(L1Algo) / 4.);
			}

			nMaskWords += sizeof(L1Algo);

			/*
			 LOG_INFO("Algo ID " << (uint) algoPacket->algoID << " Algo ProcessID " << (uint)algoPacket->processID);
			 LOG_INFO("Quality Flags " << (uint) algoPacket->qualityFlags << " Algo Flags " << (uint)algoPacket->algoFlags);

			 std::bitset<8> algoFlag((uint) algoPacket->algoFlags);
			 std::bitset<8> qualityFlag((uint) algoPacket->qualityFlags);
			 LOG_INFO("Quality Flags " << qualityFlag << " Algo Flags " << algoFlag);

			 LOG_INFO("Number of Words " << (uint) algoPacket->numberOfWords << " DS Factor " << (uint)algoPacket->downscaleFactor);
			 LOG_INFO("Online TW " << (uint)algoPacket->onlineTimeWindow);
			 LOG_INFO("L1 Data[0] " << (uint) algoPacket->l1Data[0] << " L1 Data[1] " << (uint)algoPacket->l1Data[1]);
			 */
		}

		maskPacket->referenceTimeSourceID = refTimeSourceID;
		if (!refTimeSourceID)
			maskPacket->referenceFineTime = evtRefFineTime;
		else if (refTimeSourceID == 1 && l1Info_->isL1CHODProcessed())
			maskPacket->referenceFineTime =
					(uint8_t) l1Info_->getCHODAverageTime();
//		LOG_INFO("Mask Ref Finetime " << (uint)maskPacket->referenceFineTime);
//		LOG_INFO("Mask Ref Finetime Source ID " << (uint)maskPacket->referenceTimeSourceID);
	}

	const l0::MEPFragment* const L1TPEvent =
			event->getL1ResultSubevent()->getFragment(0);
	char* payload = (char*) L1TPEvent->getPayload();
	memcpy(payload, buffer, L1DataPacketSize);
}

bool L1TriggerProcessor::writeAlgoPacket(int algoID, L1Algo* algoPacket,
		uint l0MaskID) {
	switch (algoID) {
	case 0:
		CHODAlgo::writeData(algoPacket, l0MaskID);
		return true;
	case 1:
//		RICHAlgo::writeData(algoPacket,l0MaskID);
		return true;
	case 2:
		KtagAlgo::writeData(algoPacket, l0MaskID);
		return true;
	case 3:
		LAVAlgo::writeData(algoPacket, l0MaskID);
		return true;
	case 4:
//		IRCSACAlgo::writeData(algoPacket,l0MaskID);
		return true;
	case 5:
//		StrawAlgo::writeData(algoPacket,l0MaskID);
		return true;
	case 6:
		MUV3Algo::writeData(algoPacket, l0MaskID);
		return true;
	case 7:
//		NewCHODAlgo::writeData(algoPacket,l0MaskID);
		return true;
	default:
		return false;
	}
}

}
/* namespace na62 */
