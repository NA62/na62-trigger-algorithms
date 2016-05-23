/*
 * MyOptions.h
 *
 *  Created on: Apr 11, 2014
 \*      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#pragma once
#ifndef TRIGGEROPTIONS_H_
#define TRIGGEROPTIONS_H_

#include <options/Options.h>
#include <sys/types.h>
#include <string>

/*
 * Dynamic Options
 */
#define OPTION_TRIGGER_CONFIG_FILE (char*)"triggerConfigFile"

#define OPTION_TRIGGER_XML_FILE (char*)"triggerXMLFile"

//#define OPTION_L1_BYPASS_PROBABILITY (char*)"L1BypassProbability"
//#define OPTION_L2_BYPASS_PROBABILITY (char*)"L2BypassProbability"
//
//#define OPTION_L1_REDUCTION_FACTOR  (char*)"L1ReductionFactor"
//#define OPTION_L2_REDUCTION_FACTOR  (char*)"L2ReductionFactor"
//
//#define OPTION_L1_DOWNSCALE_FACTOR  (char*)"L1DownscaleFactor"
//#define OPTION_L2_DOWNSCALE_FACTOR  (char*)"L2DownscaleFactor"
//
//#define OPTION_L1_FLAG_MODE (char*) "L1FlagMode"
//#define OPTION_L2_FLAG_MODE (char*) "L2FlagMode"
//
//#define OPTION_L1_AUTOFLAG_FACTOR (char*) "L1AutoFlagFactor"
//#define OPTION_L2_AUTOFLAG_FACTOR (char*) "L2AutoFlagFactor"
//
//#define OPTION_L1_TRIG_MASK (char*) "L1TrigMask"
//#define OPTION_L1_TRIG_LOGIC (char*) "L1TrigLogic"
//#define OPTION_L1_TRIG_FLAG (char*) "L1TrigFlag"
//#define OPTION_L1_TRIG_DS (char*) "L1TrigDS"

//#define OPTION_L1_REFERENCE_TIME (char*) "L1ReferenceTime"

// Here you can add your Options like e.g.
// #define OPTION_YOUR_OPTION_NAME (char*)"nameUsedAs--Flag"
// Then add your option to the desc object in Load below

namespace na62 {
class TriggerOptions: public Options {
public:
	TriggerOptions();
	virtual ~TriggerOptions();

	static void Load(int argc, char* argv[]) {
		fileNameOptions.push_back(OPTION_TRIGGER_CONFIG_FILE);

		desc.add_options()

		(OPTION_TRIGGER_CONFIG_FILE,
				po::value<std::string>()->default_value(
						"/etc/na62-trigger.conf"),
				"Config file for trigger specific options")

		(OPTION_TRIGGER_XML_FILE, po::value<std::string>()->default_value("/lkrpn0/RunControl/PCFarm/Trigger/config_START_RUN.cfg"),
				"XML Trigger file for trigger configuration")

				/*
				 * Define L1 - L2 Trigger Global options
				 */
//		(OPTION_L1_BYPASS_PROBABILITY, po::value<double>()->default_value(0.),
//				"Probability for every event to bypass L1 and L2 processing")
//
//		(OPTION_L2_BYPASS_PROBABILITY, po::value<double>()->default_value(0.),
//				"Probability for every event to bypass L2 processing if it did not bypass L1 already")
//
//		(OPTION_L1_REDUCTION_FACTOR, po::value<int>()->default_value(1),
//				"With this integer you can reduce the event rate going to L2 to a factor of 1/L1ReductionFactor. L1 Trigger will be processed every i event if  i++%reductionFactor==0")
//
//		(OPTION_L2_REDUCTION_FACTOR, po::value<int>()->default_value(1),
//				"With this integer you can reduce the event rate accepted by L2 to a factor of 1/L1ReductionFactor. L2 Trigger will be processed every i event if  i++%reductionFactor==0")
//
//		(OPTION_L1_DOWNSCALE_FACTOR, po::value<int>()->default_value(1),
//				"With this integer you can downscale the event rate accepted by L1 to a factor of 1/L1DownscaleFactor. L1 Trigger will accept every succeeded i event if  i++%downscaleFactor==0")
//
//		(OPTION_L2_DOWNSCALE_FACTOR, po::value<int>()->default_value(1),
//				"With this integer you can downscale the event rate accepted by L2 to a factor of 1/L2DownscaleFactor. L2 Trigger will accept every succeeded i event if  i++%downscaleFactor==0")
//
//		(OPTION_L1_FLAG_MODE, po::value<int>()->default_value(0),
//				"Enable global flagging mode (No Cut) for L1 trigger")
//
//		(OPTION_L2_FLAG_MODE, po::value<int>()->default_value(0),
//				"Enable global flagging mode (No Cut) for L2 trigger")
//
//		(OPTION_L1_AUTOFLAG_FACTOR, po::value<int>()->default_value(10),
//				"Percentage of events being flagged at L1 when is running in cutting mode")
//
//		(OPTION_L2_AUTOFLAG_FACTOR, po::value<int>()->default_value(10),
//				"Percentage of events being flagged at L2 when is running in cutting mode")
//		/*
//		 * Define L1 - L2 Trigger Local options
//		 */
//		(OPTION_L1_TRIG_MASK, po::value<std::string>()->default_value("0"),
//				"Set L1 trigger mask for L0 trigger mask")
//
//		(OPTION_L1_TRIG_LOGIC, po::value<std::string>()->default_value("0xFFFF"),
//				"Set L1 trigger logic for L0 trigger mask")
//
//		(OPTION_L1_TRIG_FLAG, po::value<std::string>()->default_value("0"),
//				"Set L1 trigger flag for L0 trigger mask")
//
//		(OPTION_L1_TRIG_DS, po::value<std::string>()->default_value("0"),
//				"Set global L1 trigger downscaling for L0 trigger mask")
//
//		(OPTION_L1_REFERENCE_TIME, po::value<int>()->default_value(0),
//				"Set L1 reference time source.")
				/*
				 * Add your Option here like following:
				 * 	(OPTION_YOUR_OPTION_NAME, po::value<std::string>()->default_value(123),
				 *		"Your comment about the option")
				 *
				 *		Or for options that need to be defined in the configuration file or at startup with --optionName=Value:
				 *	(OPTION_YOUR_OPTION_NAME, po::value<std::string>()->required(),
				 *		"Your comment about the option")
				 */

				;
	}
};

} /* namespace na62 */

#endif /* TRIGGEROPTIONS_H_ */
