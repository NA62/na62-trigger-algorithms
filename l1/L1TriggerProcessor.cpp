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

#include "L1Fragment.h"
#include "KtagAlgo.h"
#include "CHODAlgo.h"
#include "RICHAlgo.h"
#include "LAVAlgo.h"

namespace na62 {

std::atomic<uint64_t>* L1TriggerProcessor::L1Triggers_ = new std::atomic<
		uint64_t>[0xFF + 1];
std::atomic<uint64_t>* L1TriggerProcessor::L1AcceptedEventsPerL0Mask_ =
		new std::atomic<uint64_t>[16];
std::atomic<uint64_t>** L1TriggerProcessor::eventCountersByL0MaskByAlgoID_;
std::atomic<uint64_t> L1TriggerProcessor::L1InputEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1InputReducedEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1InputEventsPerBurst_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1AcceptedEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1BypassedEvents_(0);

double L1TriggerProcessor::bypassProbability;
<<<<<<< HEAD
uint L1TriggerProcessor::reductionFactor = 0;
uint L1TriggerProcessor::downscaleFactor = 0;
bool L1TriggerProcessor::flagMode = 0;
uint L1TriggerProcessor::autoFlagFactor = 0;
uint L1TriggerProcessor::referenceTimeSourceID = 0;

uint L1TriggerProcessor::numberOfEnabledAlgos[16];
uint L1TriggerProcessor::numberOfFlaggedAlgos[16];
uint L1TriggerProcessor::algoReductionFactor[16];
uint_fast16_t L1TriggerProcessor::algoEnableMask[16];
uint_fast16_t L1TriggerProcessor::algoDwScMask[16];
uint L1TriggerProcessor::algoDwScFactor[16][4];

uint_fast16_t L1TriggerProcessor::chodEnableMask = 0;
uint_fast16_t L1TriggerProcessor::richEnableMask = 0;
uint_fast16_t L1TriggerProcessor::cedarEnableMask = 0;
uint_fast16_t L1TriggerProcessor::lavEnableMask = 0;

uint_fast16_t L1TriggerProcessor::chodFlagMask = 0;
uint_fast16_t L1TriggerProcessor::richFlagMask = 0;
uint_fast16_t L1TriggerProcessor::cedarFlagMask = 0;
uint_fast16_t L1TriggerProcessor::lavFlagMask = 0;

int L1TriggerProcessor::chodProcessID[16];
int L1TriggerProcessor::richProcessID[16];
int L1TriggerProcessor::cedarProcessID[16];
int L1TriggerProcessor::lavProcessID[16];

uint L1TriggerProcessor::chodAlgorithmId;
uint L1TriggerProcessor::richAlgorithmId;
uint L1TriggerProcessor::cedarAlgorithmId;
uint L1TriggerProcessor::lavAlgorithmId;

uint_fast8_t L1TriggerProcessor::l0TrigWord = 0;
uint_fast16_t L1TriggerProcessor::l0TrigFlags = 0;

uint_fast8_t L1TriggerProcessor::chodTrigger = 0;
uint_fast8_t L1TriggerProcessor::richTrigger = 0;
uint_fast8_t L1TriggerProcessor::cedarTrigger = 0;
uint_fast8_t L1TriggerProcessor::lavTrigger = 0;
uint_fast8_t L1TriggerProcessor::l1TriggerWords[16];

L1InfoToStorage* L1TriggerProcessor::l1Info_ = L1InfoToStorage::GetInstance();
uint L1TriggerProcessor::l1ProcessID = 0;
uint L1TriggerProcessor::numberOfTriggeredL1Masks = 0;
bool L1TriggerProcessor::isAlgoEnableForAllL0Masks = 0;
bool L1TriggerProcessor::isDownscaledAndFlaggedEvent = 0;
bool L1TriggerProcessor::isReducedEvent = 0;

void L1TriggerProcessor::registerDownscalingAlgorithms() {
	uint numberOfRegisteredAlgorithms = 0;
//	cedarAlgorithmId = L1Downscaling::registerAlgorithm("CEDAR");
//	chodAlgorithmId = L1Downscaling::registerAlgorithm("CHOD");
//	richAlgorithmId = L1Downscaling::registerAlgorithm("RICH");
//	lavAlgorithmId = L1Downscaling::registerAlgorithm("LAV");
}

void L1TriggerProcessor::registerReductionAlgorithms() {
	uint numberOfRegisteredAlgorithms = 0;
//	cedarAlgorithmId = L1Reduction::registerAlgorithm("CEDAR");
//	chodAlgorithmId = L1Reduction::registerAlgorithm("CHOD");
//	richAlgorithmId = L1Reduction::registerAlgorithm("RICH");
}

bool L1TriggerProcessor::isRequestZeroSuppressedCreamData(
		uint_fast8_t l1TriggerTypeWord) {
	// add any special trigger here
	return l1TriggerTypeWord != TRIGGER_L1_BYPASS;
}

void L1TriggerProcessor::initialize(l1Struct &l1Struct) {
	// Seed for rand()
	srand(time(NULL));

	eventCountersByL0MaskByAlgoID_ = new std::atomic<uint64_t>*[16];
	for (int i = 0; i != 16; ++i) {
		eventCountersByL0MaskByAlgoID_[i] = new std::atomic<uint64_t>[4] { };
	}
	for (int i = 0; i != 0xFF + 1; i++) {
		L1Triggers_[i] = 0;
	}

	bypassProbability = l1Struct.l1Global.l1BypassProbability;
	reductionFactor = l1Struct.l1Global.l1ReductionFactor;
	downscaleFactor = l1Struct.l1Global.l1DownscaleFactor;
	flagMode = (bool) l1Struct.l1Global.l1FlagMode;
	autoFlagFactor = l1Struct.l1Global.l1AutoFlagFactor;
	referenceTimeSourceID = l1Struct.l1Global.l1ReferenceTimeSourceID;
/*
	LOG_INFO << "byPassProb " << bypassProbability << " autoFlagFactor "
			<< autoFlagFactor << " redFactor " << reductionFactor
			<< " dwscFactor " << downscaleFactor << " flagMode " << flagMode
			<< " refTimeID " << referenceTimeSourceID << ENDL;
*/
	L1Downscaling::initialize();
	L1Reduction::initialize();

	/*
	 * Initialisation of 16 L0 trigger masks
	 */
	for (int i = 0; i != 16; i++) {
		if (!i) {
			chodAlgorithmId = l1Struct.l1Mask[i].chod.configParams.l1TrigMaskID;
			richAlgorithmId = l1Struct.l1Mask[i].rich.configParams.l1TrigMaskID;
			cedarAlgorithmId =
					l1Struct.l1Mask[i].ktag.configParams.l1TrigMaskID;
			lavAlgorithmId = l1Struct.l1Mask[i].lav.configParams.l1TrigMaskID;
		} else {
			if ((chodAlgorithmId
					!= l1Struct.l1Mask[i].chod.configParams.l1TrigMaskID)
					|| (richAlgorithmId
							!= l1Struct.l1Mask[i].rich.configParams.l1TrigMaskID)
					|| (cedarAlgorithmId
							!= l1Struct.l1Mask[i].ktag.configParams.l1TrigMaskID)
					|| (lavAlgorithmId
							!= l1Struct.l1Mask[i].lav.configParams.l1TrigMaskID))
				LOG_ERROR << "Mismatch between AlgoID !!!" << ENDL;
		}
		numberOfEnabledAlgos[i] = l1Struct.l1Mask[i].numberOfEnabledAlgos;
		numberOfFlaggedAlgos[i] = l1Struct.l1Mask[i].numberOfFlaggedAlgos;
		algoReductionFactor[i] = l1Struct.l1Mask[i].maskReductionFactor;

		algoEnableMask[i] = (l1Struct.l1Mask[i].lav.configParams.l1TrigEnable
				<< l1Struct.l1Mask[i].lav.configParams.l1TrigMaskID)
				| (l1Struct.l1Mask[i].ktag.configParams.l1TrigEnable
						<< l1Struct.l1Mask[i].ktag.configParams.l1TrigMaskID)
				| (l1Struct.l1Mask[i].chod.configParams.l1TrigEnable
						<< l1Struct.l1Mask[i].chod.configParams.l1TrigMaskID);

		algoDwScMask[i] = (l1Struct.l1Mask[i].lav.configParams.l1TrigDownScale
				<< l1Struct.l1Mask[i].lav.configParams.l1TrigMaskID)
				| (l1Struct.l1Mask[i].ktag.configParams.l1TrigDownScale
						<< l1Struct.l1Mask[i].ktag.configParams.l1TrigMaskID)
				| (l1Struct.l1Mask[i].chod.configParams.l1TrigDownScale
						<< l1Struct.l1Mask[i].chod.configParams.l1TrigMaskID);

		algoDwScFactor[i][(uint) l1Struct.l1Mask[i].chod.configParams.l1TrigMaskID] =
				l1Struct.l1Mask[i].chod.configParams.l1TrigDSFactor;
		algoDwScFactor[i][(uint) l1Struct.l1Mask[i].rich.configParams.l1TrigMaskID] =
				l1Struct.l1Mask[i].rich.configParams.l1TrigDSFactor;
		algoDwScFactor[i][(uint) l1Struct.l1Mask[i].ktag.configParams.l1TrigMaskID] =
				l1Struct.l1Mask[i].ktag.configParams.l1TrigDSFactor;
		algoDwScFactor[i][(uint) l1Struct.l1Mask[i].lav.configParams.l1TrigMaskID] =
				l1Struct.l1Mask[i].lav.configParams.l1TrigDSFactor;

		l1TriggerWords[i] = 0;
/*
		std::bitset<8> enableMask(algoEnableMask[i]);
		std::bitset<8> dwscMask(algoDwScMask[i]);
		LOG_INFO << "L0 maskID " << i << " nEnabledAlgos "
				<< numberOfEnabledAlgos[i] << " enableMask " << enableMask
				<< " nFlagAlgos " << numberOfFlaggedAlgos[i] << " dwscMask "
				<< dwscMask << " reductionFactor " << algoReductionFactor[i]
				<< ENDL;
		LOG_INFO << "Downscale Factors CHOD: "
				<< algoDwScFactor[i][(uint) l1Struct.l1Mask[i].chod.configParams.l1TrigMaskID]
				<< " RICH: "
				<< algoDwScFactor[i][(uint) l1Struct.l1Mask[i].rich.configParams.l1TrigMaskID]
				<< " KTAG: "
				<< algoDwScFactor[i][(uint) l1Struct.l1Mask[i].ktag.configParams.l1TrigMaskID]
				<< " LAV: "
				<< algoDwScFactor[i][(uint) l1Struct.l1Mask[i].lav.configParams.l1TrigMaskID]
				<< ENDL;
*/
		CHODAlgo::initialize(l1Struct.l1Mask[i].chod);
//		RICHAlgo::initialize(l1Struct.l1Mask[i].rich);
		KtagAlgo::initialize(l1Struct.l1Mask[i].ktag);
		LAVAlgo::initialize(l1Struct.l1Mask[i].lav);

		chodProcessID[i] = l1Struct.l1Mask[i].chod.configParams.l1TrigProcessID;
		richProcessID[i] = l1Struct.l1Mask[i].rich.configParams.l1TrigProcessID;
		cedarProcessID[i] =
				l1Struct.l1Mask[i].ktag.configParams.l1TrigProcessID;
		lavProcessID[i] = l1Struct.l1Mask[i].lav.configParams.l1TrigProcessID;
/*
		LOG_INFO << "Processing Time Order: CHOD " << chodProcessID[i]
				<< " KTAG " << cedarProcessID[i] << " LAV " << lavProcessID[i]
				<< ENDL;
*/
		chodEnableMask |= (l1Struct.l1Mask[i].chod.configParams.l1TrigEnable
				<< i);
		richEnableMask |= (l1Struct.l1Mask[i].rich.configParams.l1TrigEnable
				<< i);
		cedarEnableMask |= (l1Struct.l1Mask[i].ktag.configParams.l1TrigEnable
				<< i);
		lavEnableMask |=
				(l1Struct.l1Mask[i].lav.configParams.l1TrigEnable << i);

		chodFlagMask |= (l1Struct.l1Mask[i].chod.configParams.l1TrigFlag << i);
		richFlagMask |= (l1Struct.l1Mask[i].rich.configParams.l1TrigFlag << i);
		cedarFlagMask |= (l1Struct.l1Mask[i].ktag.configParams.l1TrigFlag << i);
		lavFlagMask |= (l1Struct.l1Mask[i].lav.configParams.l1TrigFlag << i);
	}
/*
	LOG_INFO << "Enable Masks: CHOD " << chodEnableMask << " KTAG "
			<< cedarEnableMask << " LAV " << lavEnableMask << ENDL;

	std::bitset<16> chodEnable(chodEnableMask);
	std::bitset<16> richEnable(richEnableMask);
	std::bitset<16> cedarEnable(cedarEnableMask);
	std::bitset<16> lavEnable(lavEnableMask);
	LOG_INFO << "Enable Masks: CHOD " << chodEnable << " KTAG " << cedarEnable
			<< " LAV " << lavEnable << ENDL;

	LOG_INFO << "Flag Masks: CHOD " << chodFlagMask << " KTAG " << cedarFlagMask
			<< " LAV " << lavFlagMask << ENDL;

	std::bitset<16> chodFlag(chodFlagMask);
	std::bitset<16> richFlag(richFlagMask);
	std::bitset<16> cedarFlag(cedarFlagMask);
	std::bitset<16> lavFlag(lavFlagMask);
	LOG_INFO << "Flag Masks: CHOD " << chodFlag << " KTAG " << cedarFlag
			<< " LAV " << lavFlag << ENDL;
*/
}

uint_fast8_t L1TriggerProcessor::compute(Event* const event) {

	using namespace l0;
	DecoderHandler decoder(event);
	event->readTriggerTypeWordAndFineTime();

	L1InputEvents_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO << "L1Event number after adding 1 " << L1InputEvents_ << ENDL;
	L1InputEventsPerBurst_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO << "L1Event number per Burst after adding 1 " << L1InputEventsPerBurst_ << ENDL;

	/*
	 * Check if the event should bypass the processing
	 */
	if (event->isSpecialTriggerEvent()) {
		event->setRrequestZeroSuppressedCreamData(false);
		return TRIGGER_L1_BYPASS;
	}
	if (bypassEvent()) {
		L1BypassedEvents_.fetch_add(1, std::memory_order_relaxed);
//		LOG_INFO << "L1 ByPassed Event number after adding 1 " << L1BypassedEvents_ << ENDL;
		event->setRrequestZeroSuppressedCreamData(true);
		return TRIGGER_L1_BYPASS;
	}
	/*
	 * Check if the event fulfills the reduction option
	 *
	 */
//	LOG_INFO << "L1Reduction Factor " << reductionFactor << ENDL;
//	LOG_INFO << "Modulo " << L1InputEvents_ % reductionFactor << ENDL;
	if (L1InputEvents_ % reductionFactor != 0)
		return 0;

	L1InputReducedEvents_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO << "L1ReducedEvent number after adding 1 " << L1InputReducedEvents_ << ENDL;

	/*
	 * The event is ready to be processed
	 *
	 */
	l0TrigWord = event->getL0TriggerTypeWord();
//	LOG_INFO << "l0TriggerWord " << std::hex << (uint) l0TrigWord << std::dec << ENDL;
	l0TrigFlags = event->getTriggerFlags();
//	LOG_INFO << "l0TriggerFlags " << std::hex << (uint) l0TrigFlags << std::dec << ENDL;

	/*
	 * Store the global event timestamp taken from the reverence detector
	 */
	l0::MEPFragment* tsFragment = event->getL0SubeventBySourceIDNum(
			SourceIDManager::TS_SOURCEID_NUM)->getFragment(0);
	event->setTimestamp(tsFragment->getTimestamp());

//	const l0::MEPFragment* const L1Fragment = event->getL1Subevent()->getFragment(0);
//	const char* payload = L1Fragment->getPayload();
//	L1_BLOCK * l1Block = (L1_BLOCK *) (payload);
// Setting the new globalDownscaleFactor and globalReductionFactor in L1Block
//	uint globDownFactor = L1Builder::GetL1DownscaleFactor();
//	l1Block->globaldownscaling = globDownFactor;
//	uint globReducFactor = L1Builder::GetL1ReductionFactor();
//	l1Block->globalreduction = globReducFactor;

	uint_fast8_t l1FlagTrigger = 0;
	if (flagMode || (L1InputReducedEvents_ % autoFlagFactor == 0)) {
		l1FlagTrigger = 1;
	} else {
		l1FlagTrigger = 0;
	}

	isReducedEvent = 0;
	isDownscaledAndFlaggedEvent = 0;
	isAlgoEnableForAllL0Masks = 0;
	numberOfTriggeredL1Masks = 0;
	chodTrigger = 0;
	richTrigger = 0;
	cedarTrigger = 0;
	lavTrigger = 0;
	uint_fast8_t l1TriggerTmp;

	for (int i = 0; i != 16; i++) {
		l1TriggerTmp = 0;
		l1TriggerWords[i] = 0;
		if (l0TrigFlags & (1 << i)) {
			l1ProcessID = 0;
			if (numberOfFlaggedAlgos[i])
				l1FlagTrigger = 1;
//			LOG_INFO << "Modulo (algoReductionFactor) " << L1InputReducedEvents_ % algoReductionFactor[i]<< ENDL;
			if (L1InputReducedEvents_ % algoReductionFactor[i] != 0)
				isReducedEvent = 1;
			else
				isReducedEvent = 0;
			/*
			LOG_INFO << "isReducedEvent " << isReducedEvent << ENDL;
			LOG_INFO << "i " << i << " processID " << l1ProcessID
					<< " nEnAlgos " << numberOfEnabledAlgos[i]
					<< " l1TriggerTemp " << (uint) l1TriggerTmp
					<< " nFlagAlgos " << numberOfFlaggedAlgos[i]
					<< " l1FlagTrig " << (uint) l1FlagTrigger << ENDL;
					*/
			while (!isReducedEvent && l1ProcessID != numberOfEnabledAlgos[i]) {

				if ((chodEnableMask & (1 << i))
						&& (chodProcessID[i] == l1ProcessID)
						&& SourceIDManager::isChodActive()) {
//					LOG_INFO << "is CHOD Algo Processed? " << CHODAlgo::isAlgoProcessed() << ENDL;
					if (!CHODAlgo::isAlgoProcessed()) {
//						LOG_INFO << "I am Processing CHOD!!!!!" << ENDL;
						chodTrigger = CHODAlgo::processCHODTrigger(decoder,
								l1Info_);
//					if (chodTrigger != 0) {
//						L1Downscaling::processAlgorithm(chodAlgorithmId);
//					}
					}
					l1ProcessID++;
					l1TriggerTmp = chodTrigger;
//					printf("L1TriggerProcessor.cpp: chodTrigger %d\n",chodTrigger);
				}
				if ((chodEnableMask & l0TrigFlags) == l0TrigFlags)
					isAlgoEnableForAllL0Masks = 1;
				if (!(l1TriggerTmp & algoEnableMask[i]) && l1ProcessID
						&& !numberOfFlaggedAlgos[i])
					break;
/*
				if ((richEnableMask & (1 << i))
						&& richProcessID[i] == l1ProcessID
						&& SourceIDManager::isRhichActive()) {
					LOG_INFO << "I am Processing RICH!!!!!" << ENDL;
					richTrigger = RICHAlgo::processRICHTrigger(decoder);
//					if (richTrigger != 0) {
//						L1Downscaling::processAlgorithm(richAlgorithmId);
//					}
					l1ProcessID++;
					l1TriggerTmp = richTrigger;
					printf("L1TriggerProcessor.cpp: richTrigger %d\n",
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
//					LOG_INFO << "is Ktag Algo Processed? " << KtagAlgo::isAlgoProcessed() << ENDL;
					if (!KtagAlgo::isAlgoProcessed()) {
//						LOG_INFO << "I am Processing KTAG!!!!!" << ENDL;
						cedarTrigger = KtagAlgo::processKtagTrigger(decoder,
								l1Info_);
//					if (cedarTrigger != 0) {
//						L1Downscaling::processAlgorithm(cedarAlgorithmId);
//					}
					}
					l1ProcessID++;
					l1TriggerTmp = cedarTrigger;
//					printf("L1TriggerProcessor.cpp: cedarTrigger %d\n",cedarTrigger);
				}
				if ((cedarEnableMask & l0TrigFlags) == l0TrigFlags)
					isAlgoEnableForAllL0Masks = 1;
				if (!(l1TriggerTmp & algoEnableMask[i]) && l1ProcessID
						&& !numberOfFlaggedAlgos[i])
					break;

				if ((lavEnableMask & (1 << i)) && lavProcessID[i] == l1ProcessID
						&& SourceIDManager::isLavActive()) {
//					LOG_INFO << "is LAV Algo Processed? "<< LAVAlgo::isAlgoProcessed() << ENDL;
					if (!LAVAlgo::isAlgoProcessed()) {
//						LOG_INFO << "I am Processing LAV!!!!!" << ENDL;
						lavTrigger = LAVAlgo::processLAVTrigger(decoder,
								l1Info_);
//					if (lavTrigger != 0) {
//						L1Downscaling::processAlgorithm(lavAlgorithmId);
//					}
					}
					l1ProcessID++;
					l1TriggerTmp = lavTrigger;
//					printf("L1TriggerProcessor.cpp: lavTrigger %d\n",lavTrigger);
				}
				if ((lavEnableMask & l0TrigFlags) == l0TrigFlags)
					isAlgoEnableForAllL0Masks = 1;
				if (!(l1TriggerTmp & algoEnableMask[i]) && l1ProcessID
						&& !numberOfFlaggedAlgos[i])
					break;
			}
			/*
			 * L1 trigger word calculation
			 */
			l1TriggerTmp = (lavTrigger << 3) | (cedarTrigger << 2)
					| (richTrigger << 1) | chodTrigger;
//			printf("L1TriggerProcessor.cpp: l1Trigger (!!TMP!!) %x\n",l1TriggerTmp);
			/*
			LOG_INFO << "isReducedEvent " << isReducedEvent << " EnableMask "
					<< algoEnableMask[i] << " trigTmp & EnableMask "
					<< (l1TriggerTmp & algoEnableMask[i]) << " l1FlagTrig "
					<< (uint) l1FlagTrigger << ENDL;
					*/
			if (!isReducedEvent
					&& (!algoEnableMask[i]
							|| ((l1TriggerTmp & algoEnableMask[i])
									== algoEnableMask[i]) || l1FlagTrigger)) {
				L1AcceptedEventsPerL0Mask_[i].fetch_add(1,
						std::memory_order_relaxed);
				/*
				LOG_INFO << "L1 Accepted Event Per L0 mask " << i
						<< " number after adding 1 "
						<< L1AcceptedEventsPerL0Mask_[i] << ENDL;
				LOG_INFO << "TrigTmp " << (uint) l1TriggerTmp << " dwscMask "
						<< (uint) algoDwScMask[i] << ENDL;
				LOG_INFO << "downscale Factor "
						<< algoDwScFactor[i][__builtin_ctz(
								(uint) algoDwScMask[i])] << ENDL;
				LOG_INFO << "Modulo "
						<< L1AcceptedEventsPerL0Mask_[i]
								% algoDwScFactor[i][__builtin_ctz(
										(uint) algoDwScMask[i])]<< ENDL;
										*/
				if ((l1TriggerTmp & algoDwScMask[i])
						&& (++(eventCountersByL0MaskByAlgoID_[i][__builtin_ctz(
								(uint) algoDwScMask[i])])
								% algoDwScFactor[i][__builtin_ctz(
										(uint) algoDwScMask[i])] != 0)) {
					l1TriggerWords[i] = 0;
//					isDownscaledAndFlaggedEvent += (numberOfFlaggedAlgos[i] != 0);
					isDownscaledAndFlaggedEvent += ((uint) l1FlagTrigger);
					/*
					LOG_INFO << "flagTrig " << (uint) l1FlagTrigger
							<< " isDownscaledAndFlaggedEvent "
							<< isDownscaledAndFlaggedEvent << ENDL;
							*/
				} else
					l1TriggerWords[i] = ((l1TriggerTmp & algoEnableMask[i])
							== algoEnableMask[i]);
			} else
				l1TriggerWords[i] = 0;
//			printf("L1TriggerProcessor.cpp: l1Trigger %x\n", l1TriggerWords[i]);
		}
		if (__builtin_popcount((uint) l1TriggerWords[i]))
			numberOfTriggeredL1Masks++;
	}

//	printf("Summary of Triggered Masks: %d\n", numberOfTriggeredL1Masks);
//	for (int i = 0; i != 16; i++) printf("Summary of Trigger Words: l1Trigger %x\n", l1TriggerWords[i]);

	if (l1Info_->isL1CHODProcessed()) l1Info_->resetL1CHODProcessed();

	if (CHODAlgo::isAlgoProcessed())
		CHODAlgo::resetAlgoProcessed();
//	if(RICHAlgo::isAlgoProcessed()) RICHAlgo::resetAlgoProcessed();
	if (KtagAlgo::isAlgoProcessed())
		KtagAlgo::resetAlgoProcessed();
	if (LAVAlgo::isAlgoProcessed())
		LAVAlgo::resetAlgoProcessed();

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
	uint_fast8_t l1Trigger = 0;
/*
	LOG_INFO << (uint) l1FlagTrigger << " " << isDownscaledAndFlaggedEvent
			<< " " << ((uint) l1FlagTrigger && !isDownscaledAndFlaggedEvent)
			<< ENDL;
			*/
	l1Trigger = (((uint) l1FlagTrigger && !isDownscaledAndFlaggedEvent) << 7) | (numberOfTriggeredL1Masks != 0);
//	l1Trigger = (l1FlagTrigger << 7) | (lavTrigger << 3) | (cedarTrigger << 2) | (richTrigger << 1) | chodTrigger;
//	l1Trigger = (cedarTrigger != 0 && chodTrigger != 0);
//	printf("L1TriggerProcessor.cpp: !!!!!!!! Final l1Trigger %x\n", l1Trigger);

	L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed); // The second 8 bits are the L1 trigger type word

	if (l1Trigger != 0) {
		/*
		 * TODO: Pass this counter to Nicolas to print number of events before downscaling
		 */
		L1AcceptedEvents_.fetch_add(1, std::memory_order_relaxed);
//		LOG_INFO << "L1 Accepted Event number after adding 1 " << L1AcceptedEvents_ << ENDL;

		//Global L1 downscaling
//		LOG_INFO << "L1Downscale Factor " << downscaleFactor << ENDL;
//		LOG_INFO << "Modulo " << L1AcceptedEvents_ % downscaleFactor << ENDL;
		if (L1AcceptedEvents_ % downscaleFactor != 0)
			return 0;
		/*
		 * Decision whether or not to request zero suppressed data from the creams
		 */
		event->setRrequestZeroSuppressedCreamData(true);
		event->setProcessingID(0); // 0 indicates raw data as collected from the detector
	}
	return l1Trigger;
}

}
/* namespace na62 */
