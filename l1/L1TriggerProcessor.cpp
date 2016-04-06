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
#include "L1Downscaling.h"
#include "L1Reduction.h"
#include "L1Fragment.h"
#include "KtagAlgo.h"
#include "CHODAlgo.h"
#include "RICHAlgo.h"
#include "LAVAlgo.h"

namespace na62 {

L1InfoToStorage* L1TriggerProcessor::l1Info_ = L1InfoToStorage::GetInstance();
double L1TriggerProcessor::bypassProbability;
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

void L1TriggerProcessor::initialize(double _bypassProbability) {
	// Seed for rand()
	srand(time(NULL));

	bypassProbability = _bypassProbability;

	L1Downscaling::initialize();
	L1Reduction::initialize();
}

bool L1TriggerProcessor::isRequestZeroSuppressedCreamData(
		uint_fast8_t l1TriggerTypeWord) {
	// add any special trigger here
	return l1TriggerTypeWord != TRIGGER_L1_BYPASS;
}

uint_fast8_t L1TriggerProcessor::compute(Event* const event) {

	using namespace l0;
	DecoderHandler decoder(event);

//	event->readTriggerTypeWordAndFineTime();

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

	/*
	 * Check if the event should bypass the processing
	 */
	if (bypassEvent() || event->isSpecialTriggerEvent()) {
		// Request zero suppressed CREAM data for bypassed events?
		//event->setRrequestZeroSuppressedCreamData(
		//isRequestZeroSuppressedCreamData(TRIGGER_L1_BYPASS));

//		l1Block->triggerword = TRIGGER_L1_BYPASS;
		return TRIGGER_L1_BYPASS;
	}

	uint_fast8_t l1Trigger = 0;
	uint_fast8_t cedarTrigger = 0;
	uint_fast8_t chodTrigger = 0;
	uint_fast8_t richTrigger = 0;
	uint_fast8_t lavTrigger = 0;

	if (SourceIDManager::isChodActive()) {
		chodTrigger = CHODAlgo::processCHODTrigger(decoder,l1Info_);
		if (chodTrigger != 0) {
			L1Downscaling::processAlgorithm(chodAlgorithmId);
		}
	}
//	printf("L1TriggerProcessor.cpp: chodTrigger %d\n", chodTrigger);

//	if (SourceIDManager::isCedarActive() && l1Info_->isL1CHODProcessed()) {
	if (SourceIDManager::isCedarActive()) {
		cedarTrigger = KtagAlgo::processKtagTrigger(decoder,l1Info_);
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
		lavTrigger = LAVAlgo::processLAVTrigger(decoder,l1Info_);
		if (lavTrigger != 0) {
			L1Downscaling::processAlgorithm (lavAlgorithmId);
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
	l1Trigger = (l1Trigger << 7) | (lavTrigger << 3) | (cedarTrigger << 2) | (richTrigger << 1) | chodTrigger;
//	l1Trigger = (cedarTrigger != 0 && chodTrigger != 0);
//	printf("L1TriggerProcessor.cpp: l1Trigger %d\n",l1Trigger);
	/*
	 * Decision whether or not to request zero suppressed data from the creams
	 */
	event->setRrequestZeroSuppressedCreamData(
			isRequestZeroSuppressedCreamData(l1Trigger));
	event->setProcessingID(0); // 0 indicates raw data as collected from the detector

//	l1Block->triggerword = l1Trigger;

	return l1Trigger;
}

}
/* namespace na62 */
