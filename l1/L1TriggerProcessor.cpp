/*
 * L1TriggerProcessor.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#include "L1TriggerProcessor.h"
#include <options/Logging.h>

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
std::atomic<uint64_t> L1TriggerProcessor::L1InputEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1InputReducedEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1InputEventsPerBurst_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1AcceptedEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1BypassedEvents_(0);
std::atomic<uint64_t> L1TriggerProcessor::L1Requests_(0);
std::atomic<uint64_t>** L1TriggerProcessor::L1ProcessingTimeVsEvtNumber_;
uint L1TriggerProcessor::reductionFactor_ = 0;
uint L1TriggerProcessor::downscaleFactor_ = 0;
bool L1TriggerProcessor::flagMode_ = 0;
uint L1TriggerProcessor::autoFlagFactor_ = 0;

L1InfoToStorage* L1TriggerProcessor::l1Info_ = L1InfoToStorage::GetInstance();
double L1TriggerProcessor::bypassProbability;
uint_fast8_t L1TriggerProcessor::l0TrigWord = 0;
uint_fast16_t L1TriggerProcessor::l0TrigFlags = 0;
uint_fast16_t L1TriggerProcessor::l1TrigMask = 0;
uint_fast16_t L1TriggerProcessor::l1TrigLogic = 0;
uint_fast16_t L1TriggerProcessor::l1TrigFlag = 0;
uint_fast16_t L1TriggerProcessor::l1TrigDS = 0;
uint L1TriggerProcessor::l1ReferenceTimeSource = 0;

uint L1TriggerProcessor::cedarAlgorithmId;
uint L1TriggerProcessor::chodAlgorithmId;
uint L1TriggerProcessor::richAlgorithmId;
uint L1TriggerProcessor::lavAlgorithmId;

void L1TriggerProcessor::registerDownscalingAlgorithms() {
	uint numberOfRegisteredAlgorithms = 0;
	cedarAlgorithmId = L1Downscaling::registerAlgorithm("CEDAR");
	chodAlgorithmId = L1Downscaling::registerAlgorithm("CHOD");
	richAlgorithmId = L1Downscaling::registerAlgorithm("RICH");
	lavAlgorithmId = L1Downscaling::registerAlgorithm("LAV");
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

uint_fast8_t L1TriggerProcessor::compute(Event* const event) {

	using namespace l0;
	DecoderHandler decoder(event);
	event->readTriggerTypeWordAndFineTime();

	L1InputEvents_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO<< "L1Event number after adding 1 " << L1InputEvents_ << ENDL;
	L1InputEventsPerBurst_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO<< "L1Event number per Burst after adding 1 " << L1InputEventsPerBurst_ << ENDL;

	/*
	 * Check if the event should bypass the processing
	 */
	if (event->isSpecialTriggerEvent()) {
		event->setRrequestZeroSuppressedCreamData(false);
		return TRIGGER_L1_BYPASS;
	}
	if (bypassEvent()) {
		/*
		 * TODO: What about LKr if event is bypassed, do you want the LKr zero suppression?
		 */
		L1BypassedEvents_.fetch_add(1, std::memory_order_relaxed);
//		LOG_INFO<< "L1 ByPassed Event number after adding 1 " << L1BypassedEvents_ << ENDL;
		event->setRrequestZeroSuppressedCreamData(true);
		return TRIGGER_L1_BYPASS;
	}
	/*
	 * Check if the event fulfills the reduction option
	 *
	 */
//	LOG_INFO<< "L1Reduction Factor " << reductionFactor_ << ENDL;
//	LOG_INFO<< "Modulo " << L1InputEvents_ % reductionFactor_ << ENDL;
	if (L1InputEvents_ % reductionFactor_ != 0) return 0;

	L1InputReducedEvents_.fetch_add(1, std::memory_order_relaxed);
//	LOG_INFO<< "L1ReducedEvent number after adding 1 " << L1InputReducedEvents_ << ENDL;

	/*
	 * The event is ready to be processed
	 *
	 */
	l0TrigWord = event->getL0TriggerTypeWord();
//	LOG_INFO<< "l0TriggerWord " << std::hex << (uint)l0TrigWord << std::dec << ENDL;
	l0TrigFlags = event->getTriggerFlags();
//	LOG_INFO<< "l0TriggerFlags " << std::hex << (uint)l0TrigFlags << std::dec << ENDL;

	/*
	 * Store the global event timestamp taken from the reverence detector
	 */
	l0::MEPFragment* tsFragment = event->getL0SubeventBySourceIDNum(
			SourceIDManager::TS_SOURCEID_NUM)->getFragment(0);
	event->setTimestamp(tsFragment->getTimestamp());

//	const l0::MEPFragment* const L1Fragment =
//			event->getL1Subevent()->getFragment(0);
//
//	const char* payload = L1Fragment->getPayload();
//	L1_BLOCK * l1Block = (L1_BLOCK *) (payload);

// Setting the new globalDownscaleFactor and globalReductionFactor in L1Block

//	uint globDownFactor = L1Builder::GetL1DownscaleFactor();
//	l1Block->globaldownscaling = globDownFactor;

//	uint globReducFactor = L1Builder::GetL1ReductionFactor();
//	l1Block->globalreduction = globReducFactor;

	uint_fast8_t l1FlagTrigger = 0;
	if (flagMode_ || (L1InputReducedEvents_ % autoFlagFactor_ == 0)) {
		l1FlagTrigger = 1;
	} else {
		l1FlagTrigger = 0;
	}

	uint_fast8_t l1Trigger = 0;
	uint_fast8_t cedarTrigger = 0;
	uint_fast8_t chodTrigger = 0;
	uint_fast8_t richTrigger = 0;
	uint_fast8_t lavTrigger = 0;

	if (SourceIDManager::isChodActive()) {
		chodTrigger = CHODAlgo::processCHODTrigger(decoder, l1Info_);
		if (chodTrigger != 0) {
			L1Downscaling::processAlgorithm(chodAlgorithmId);
		}
	}
//	printf("L1TriggerProcessor.cpp: chodTrigger %d\n", chodTrigger);

//	if (SourceIDManager::isCedarActive() && l1Info_->isL1CHODProcessed()) {
	if (SourceIDManager::isCedarActive()) {
		cedarTrigger = KtagAlgo::processKtagTrigger(decoder, l1Info_);
		if (cedarTrigger != 0) {
			L1Downscaling::processAlgorithm(cedarAlgorithmId);
		}
	}
//	printf("L1TriggerProcessor.cpp: cedarTrigger %d\n", cedarTrigger);

//	if (SourceIDManager::isRhichActive()) {
//		richTrigger = RICHAlgo::processRICHTrigger(decoder);
//		if (richTrigger != 0) {
//			L1Downscaling::processAlgorithm(richAlgorithmId);
//		}
//	}
//	printf("L1TriggerProcessor.cpp: richTrigger %d\n", richTrigger);

//	if (SourceIDManager::isLavActive() && l1Info_->isL1CHODProcessed()) {
	if (SourceIDManager::isLavActive()) {
		lavTrigger = LAVAlgo::processLAVTrigger(decoder, l1Info_);
		if (lavTrigger != 0) {
			L1Downscaling::processAlgorithm(lavAlgorithmId);
		}
	}
//	printf("L1TriggerProcessor.cpp: lavTrigger %d\n", lavTrigger);

	l1Info_->resetL1CHODProcessed();
//	LOG_INFO << "Reset L1CHODProcessed boolean " << l1Info_->isL1CHODProcessed() << ENDL;

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
//	l1Trigger = cedarTrigger;
//	l1Trigger = chodTrigger;
//	l1Trigger = richTrigger;
	l1Trigger = (l1FlagTrigger << 7) | (lavTrigger << 3) | (cedarTrigger << 2)
			| (richTrigger << 1) | chodTrigger;
//	l1Trigger = (cedarTrigger != 0 && chodTrigger != 0);
//	printf("L1TriggerProcessor.cpp: l1Trigger %x\n", l1Trigger);

	L1Triggers_[l1Trigger].fetch_add(1, std::memory_order_relaxed); // The second 8 bits are the L1 trigger type word

	if (l1Trigger != 0) {
		L1AcceptedEvents_.fetch_add(1, std::memory_order_relaxed);
//		LOG_INFO<< "L1 Accepted Event number after adding 1 " << L1AcceptedEvents_ << ENDL;

		//Global L1 downscaling
//		LOG_INFO<< "L1Downscale Factor " << downscaleFactor_ << ENDL;
//		LOG_INFO<< "Modulo " << L1AcceptedEvents_ % downscaleFactor_ << ENDL;
		if (L1AcceptedEvents_ % downscaleFactor_ != 0) return 0;
	}
	/*
	 * Decision whether or not to request zero suppressed data from the creams
	 */
	event->setRrequestZeroSuppressedCreamData(true);
	event->setProcessingID(0); // 0 indicates raw data as collected from the detector

//	l1Block->triggerword = l1Trigger;

	return l1Trigger;
}

}
/* namespace na62 */
