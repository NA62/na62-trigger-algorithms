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

// Here you can add your Options like e.g.
// #define OPTION_YOUR_OPTION_NAME (char*)"nameUsedAs--Flag"
// Then add your option to the desc object in Load below

namespace na62 {
class TriggerOptions: public Options {
public:
	TriggerOptions();
	virtual ~TriggerOptions();

	static void Load(int, char**) {
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
