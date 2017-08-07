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

#define OPTION_ACTIVE_L0_MASKS (char*)"activeL0MaskIDs"

//#define OPTION_CHOD_CONFIG (char*)"chodConfig"
//#define OPTION_IRCSAC_CONFIG (char*)"ircsacConfig"
//#define OPTION_LAV_CONFIG (char*)"lavConfig"
//#define OPTION_MUV_CONFIG (char*)"muvConfig"
//#define OPTION_NEWCHOD_CONFIG (char*)"newchodConfig"
//#define OPTION_RICH_CONFIG (char*)"richConfig"
//#define OPTION_STRAW_CONFIG (char*)"strawConfig"


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

		(OPTION_TRIGGER_XML_FILE,
				po::value<std::string>()->default_value(
						"/na62server/RunControl/PCFarm/Trigger/config_START_RUN.cfg"),
				"XML Trigger file for trigger configuration")

		(OPTION_ACTIVE_L0_MASKS, po::value<std::string>()->default_value("99"),
				"List of Active L0 Masks to be used")

//		(OPTION_CHOD_CONFIG, po::value<std::string>()->default_value("/workspace/na62-trigger-algorithms/l1/chod_algorithm/config/CHOD.2017.om.conf"),
//				"Path of the configuration file")
//		(OPTION_IRCSAC_CONFIG, po::value<std::string>()->default_value("/workspace/na62-trigger-algorithms/l1/ircsac_algorithm/config/IRCSAC.2017.om.conf"),
//				"Path of the configuration file")
////		(OPTION_LAV_CONFIG, po::value<std::string>()->default_value("/workspace/na62-trigger-algorithms/l1/lav_algorithm/config/LAV.2017.om.conf"),
////				"LAV Path of the configuration file")
//		(OPTION_LAV_CONFIG, po::value<std::string>()->required(),
//				"LAV Path of the configuration file")
//		(OPTION_MUV_CONFIG, po::value<std::string>()->default_value("/workspace/na62-trigger-algorithms/l1/muv_algorithm/config/MUV3.2017.om.conf"),
//				"Path of the configuration file")
//		(OPTION_NEWCHOD_CONFIG, po::value<std::string>()->default_value("/workspace/na62-trigger-algorithms/l1/newchod_algorithm/config/NewCHOD.2017.om.conf"),
//				"Path of the configuration file")
//		(OPTION_RICH_CONFIG, po::value<std::string>()->default_value("/workspace/na62-trigger-algorithms/l1/rich_algorithm/config/RICH.conf"),
//				"Path of the configuration file")
//		(OPTION_STRAW_CONFIG, po::value<std::string>()->default_value("/workspace/na62-trigger-algorithms/l1/straw_algorithm/config/Spectrometer.2017.om.conf"),
//				"Path of the configuration file")


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
