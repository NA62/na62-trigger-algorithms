/*
 * HLTriggerManager.cpp
 *
 *  Created on: 19 Apr 2016
 *      Author: romano
 */

#include "HLTriggerManager.h"
#include "options/Logging.h"
#include "../options/TriggerOptions.h"

namespace na62 {

HLTriggerManager::HLTriggerManager() {
	// TODO Auto-generated constructor stub
}

HLTriggerManager::~HLTriggerManager() {
	// TODO Auto-generated destructor stub
}

void HLTriggerManager::createXMLFile() {
	HLTStruct* test = new HLTStruct();

	test->l1.l1Global.l1BypassProbability = 0.;
	test->l1.l1Global.l1AutoFlagFactor = 10;
	test->l1.l1Global.l1ReductionFactor = 1;
	test->l1.l1Global.l1DownscaleFactor = 1;
	test->l1.l1Global.l1FlagMode = 0;
	test->l1.l1Global.l1ReferenceTimeSourceID = 0;
	test->l2.l2Global.l2BypassProbability = 0.;
	test->l2.l2Global.l2AutoFlagFactor = 10;
	test->l2.l2Global.l2ReductionFactor = 1;
	test->l2.l2Global.l2DownscaleFactor = 1;
	test->l2.l2Global.l2FlagMode = 1;
	test->l2.l2Global.l2ReferenceTimeSourceID = 1;

	for (int i = 0; i != 16; i++) {

		test->l1.l1Mask[i].rich.configParams.l1TrigMaskID = 1;
		test->l1.l1Mask[i].rich.configParams.l1TrigLogic = 1;
		test->l1.l1Mask[i].rich.configParams.l1TrigDownScale = 0;
		test->l1.l1Mask[i].rich.configParams.l1TrigDSFactor = 1;
		test->l1.l1Mask[i].rich.configParams.l1TrigEnable = 0;
		test->l1.l1Mask[i].rich.configParams.l1TrigProcessID = -1;
		test->l1.l1Mask[i].rich.configParams.l1TrigFlag = 0;
		test->l1.l1Mask[i].rich.configParams.l1TrigRefTimeSourceID = 0;
		test->l1.l1Mask[i].rich.configParams.l1TrigOnlineTimeWindow = -1.;

		test->l1.l1Mask[i].ktag.configParams.l1TrigMaskID = 2;
		test->l1.l1Mask[i].ktag.configParams.l1TrigLogic = 1;
		test->l1.l1Mask[i].ktag.configParams.l1TrigDownScale = 0;
		test->l1.l1Mask[i].ktag.configParams.l1TrigDSFactor = 1;

		if (i != 1 && i != 2 && i != 3) {
			test->l1.l1Mask[i].ktag.configParams.l1TrigEnable = 0;
			test->l1.l1Mask[i].ktag.configParams.l1TrigProcessID = -1;
			test->l1.l1Mask[i].ktag.configParams.l1TrigFlag = 0;
			test->l1.l1Mask[i].ktag.configParams.l1TrigRefTimeSourceID = 0;
			test->l1.l1Mask[i].ktag.configParams.l1TrigOnlineTimeWindow = -1.;
		}
		if (i == 1 || i == 3) {
			test->l1.l1Mask[i].ktag.configParams.l1TrigEnable = 1;
			test->l1.l1Mask[i].ktag.configParams.l1TrigProcessID = 0;
			test->l1.l1Mask[i].ktag.configParams.l1TrigFlag = 0;
			test->l1.l1Mask[i].ktag.configParams.l1TrigRefTimeSourceID = 0;
			test->l1.l1Mask[i].ktag.configParams.l1TrigOnlineTimeWindow = 5.;
		}
		if (i == 2) {
			test->l1.l1Mask[i].ktag.configParams.l1TrigEnable = 1;
			test->l1.l1Mask[i].ktag.configParams.l1TrigProcessID = 0;
			test->l1.l1Mask[i].ktag.configParams.l1TrigFlag = 1;
			test->l1.l1Mask[i].ktag.configParams.l1TrigRefTimeSourceID = 0;
			test->l1.l1Mask[i].ktag.configParams.l1TrigOnlineTimeWindow = 5.;
		}

		test->l1.l1Mask[i].chod.configParams.l1TrigMaskID = 0;
		test->l1.l1Mask[i].chod.configParams.l1TrigLogic = 1;
		test->l1.l1Mask[i].chod.configParams.l1TrigDownScale = 0;
		test->l1.l1Mask[i].chod.configParams.l1TrigDSFactor = 1;

		/*
		 if (i == 2 || i == 3) {
		 test->l1.l1Mask[i].chod.configParams.l1TrigDownScale = 1;
		 test->l1.l1Mask[i].chod.configParams.l1TrigDSFactor = 2;
		 }
		 */
		if (i != 2 && i != 3) {
			test->l1.l1Mask[i].chod.configParams.l1TrigEnable = 0;
			test->l1.l1Mask[i].chod.configParams.l1TrigProcessID = -1;
			test->l1.l1Mask[i].chod.configParams.l1TrigFlag = 0;
			test->l1.l1Mask[i].chod.configParams.l1TrigRefTimeSourceID = 0;
			test->l1.l1Mask[i].chod.configParams.l1TrigOnlineTimeWindow = -1.;
//			test->l1.l1Mask[i].chod.configParams.l1TrigDownScale = 0;
//			test->l1.l1Mask[i].chod.configParams.l1TrigDSFactor = 1;
		}
		if (i == 2) {
			test->l1.l1Mask[i].chod.configParams.l1TrigEnable = 1;
			test->l1.l1Mask[i].chod.configParams.l1TrigProcessID = 1;
			test->l1.l1Mask[i].chod.configParams.l1TrigFlag = 1;
			test->l1.l1Mask[i].chod.configParams.l1TrigRefTimeSourceID = 0;
			test->l1.l1Mask[i].chod.configParams.l1TrigOnlineTimeWindow = 10.;
		}
		if (i == 3) {
			test->l1.l1Mask[i].chod.configParams.l1TrigEnable = 1;
			test->l1.l1Mask[i].chod.configParams.l1TrigProcessID = 1;
			test->l1.l1Mask[i].chod.configParams.l1TrigFlag = 0;
			test->l1.l1Mask[i].chod.configParams.l1TrigRefTimeSourceID = 0;
			test->l1.l1Mask[i].chod.configParams.l1TrigOnlineTimeWindow = 10.;
		}

		test->l1.l1Mask[i].lav.configParams.l1TrigMaskID = 3;
		test->l1.l1Mask[i].lav.configParams.l1TrigLogic = 0;
		test->l1.l1Mask[i].lav.configParams.l1TrigDownScale = 0;
		test->l1.l1Mask[i].lav.configParams.l1TrigDSFactor = 1;

		if (i != 2 && i != 3) {
			test->l1.l1Mask[i].lav.configParams.l1TrigEnable = 0;
			test->l1.l1Mask[i].lav.configParams.l1TrigProcessID = -1;
			test->l1.l1Mask[i].lav.configParams.l1TrigFlag = 0;
			test->l1.l1Mask[i].lav.configParams.l1TrigRefTimeSourceID = 0;
			test->l1.l1Mask[i].lav.configParams.l1TrigOnlineTimeWindow = -1.;
		}
		if (i == 2) {
			test->l1.l1Mask[i].lav.configParams.l1TrigEnable = 1;
			test->l1.l1Mask[i].lav.configParams.l1TrigProcessID = 2;
			test->l1.l1Mask[i].lav.configParams.l1TrigFlag = 1;
			test->l1.l1Mask[i].lav.configParams.l1TrigRefTimeSourceID = 0;
			test->l1.l1Mask[i].lav.configParams.l1TrigOnlineTimeWindow = 20.;
		}
		if (i == 3) {
			test->l1.l1Mask[i].lav.configParams.l1TrigEnable = 1;
			test->l1.l1Mask[i].lav.configParams.l1TrigProcessID = 2;
			test->l1.l1Mask[i].lav.configParams.l1TrigFlag = 0;
			test->l1.l1Mask[i].lav.configParams.l1TrigRefTimeSourceID = 0;
			test->l1.l1Mask[i].lav.configParams.l1TrigOnlineTimeWindow = 20.;
		}
		if (i != 1 && i != 2 && i != 3) {
			test->l1.l1Mask[i].numberOfEnabledAlgos = 0; // No L1 Triggers enabled
			test->l1.l1Mask[i].numberOfFlaggedAlgos = 0; // No L1 Triggers to be processed and flagged
		}
		if (i == 1) {
			test->l1.l1Mask[i].numberOfEnabledAlgos = 1;
			test->l1.l1Mask[i].numberOfFlaggedAlgos = 0;
		}
		if (i == 2) {
			test->l1.l1Mask[i].numberOfEnabledAlgos = 3;
			test->l1.l1Mask[i].numberOfFlaggedAlgos = 3;
		}
		if (i == 3) {
			test->l1.l1Mask[i].numberOfEnabledAlgos = 3;
			test->l1.l1Mask[i].numberOfFlaggedAlgos = 0;
		}
		if (!i)
			test->l1.l1Mask[i].maskReductionFactor = 5;
		else
			test->l1.l1Mask[i].maskReductionFactor = 1;

		/*
		 test->l1.l1Mask[i].l1AlgoEnableMask =
		 (test->l1.l1Mask[i].lav.configParams.l1TrigEnable
		 << test->l1.l1Mask[i].lav.configParams.l1TrigMaskID)
		 | (test->l1.l1Mask[i].ktag.configParams.l1TrigEnable
		 << test->l1.l1Mask[i].ktag.configParams.l1TrigMaskID)
		 | (test->l1.l1Mask[i].chod.configParams.l1TrigEnable
		 << test->l1.l1Mask[i].chod.configParams.l1TrigMaskID);

		 test->l1.l1Mask[i].l1AlgoDwScMask =
		 (test->l1.l1Mask[i].lav.configParams.l1TrigDownScale
		 << test->l1.l1Mask[i].lav.configParams.l1TrigMaskID)
		 | (test->l1.l1Mask[i].ktag.configParams.l1TrigDownScale
		 << test->l1.l1Mask[i].ktag.configParams.l1TrigMaskID)
		 | (test->l1.l1Mask[i].chod.configParams.l1TrigDownScale
		 << test->l1.l1Mask[i].chod.configParams.l1TrigMaskID);
		 */
	}

	const char* filename =
			"/Users/romano/Desktop/workspace/na62-trigger-algorithms/struct/HLTConfParams.xml";
	xml_create_HLTStruct(test, filename);
}

void HLTriggerManager::fillStructFromXMLFile(HLTStruct &HLTStruct) {

	if (xml_read_file_HLTStruct(OPTION_TRIGGER_XML_FILE)== -1)
		LOG_INFO << "Fatal error: " << xml_getLastFatalError_HLTStruct() << ENDL;

	xml_apply_HLTStruct(&HLTStruct);

}

} /* namespace na62 */
