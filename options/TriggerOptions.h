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

#define OPTION_L1_BYPASS_PROBABILITY (char*)"L1BypassProbability"
#define OPTION_L1_BYPASS_TRIGGER_WORD (char*)"L1BypassTriggerWord"

#define OPTION_L2_BYPASS_PROBABILITY (char*)"L2BypassProbability"
#define OPTION_L2_BYPASS_TRIGGER_WORD (char*)"L2BypassTriggerWord"

// Here you can add your Options like e.g.
// #define OPTION_YOUR_OPTION_NAME (char*)"nameUsedAs--Flag"
// Then add your option to the desc opbject in Load below

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

<<<<<<< HEAD
				(OPTION_L1_BYPASS_PROBABILITY, po::value<double>()->default_value(0.), "Probability for every event to bypass L1 and L2 processing")

				(OPTION_L1_BYPASS_TRIGGER_WORD, po::value<int>()->default_value(0xFF), "L1 trigger type word for events that were passed by")

				(OPTION_L2_BYPASS_PROBABILITY, po::value<double>()->default_value(0.), "Probability for every event to bypass L2 processing if it did not bypass L1 already")

				(OPTION_L2_BYPASS_TRIGGER_WORD, po::value<int>()->default_value(0xFF), "L2 trigger type word for events that were passed by")
=======
				(OPTION_L1_BYPASS_PROBABILITY, po::value<double>()->default_value(0.), "Probability for every event to bypass L1 processing")

				(OPTION_L1_BYPASS_TRIGGER_WORD, po::value<uint>()->default_value(0xFF), "L1 trigger type word for events that were passed by")

				(OPTION_L2_BYPASS_PROBABILITY, po::value<double>()->default_value(0.), "Probability for every event to bypass L2 processing")

				(OPTION_L2_BYPASS_TRIGGER_WORD, po::value<uint>()->default_value(0xFF), "L2 trigger type word for events that were passed by")
>>>>>>> Trigger bypassing implemented


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
