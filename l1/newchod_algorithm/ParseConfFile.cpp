/*
 * ParseConfFile.cpp
 *
 *  Created on: 13 May 2016
 *      Author: lorenza
 */

#include "../../common/ConfFileReader.h"
#include "ParsConfFile.h"

#include <iostream>
#include <vector>
#include <options/Logging.h>
#include <string.h>

#include <sys/time.h>
#include "../../options/TriggerOptions.h"

NewCHODParsConfFile* NewCHODParsConfFile::theInstance = nullptr;

NewCHODParsConfFile::NewCHODParsConfFile() {

//	LOG_INFO("In NewCHOD ParseConfFile");

//	ConfFileReader fileName_("/workspace/na62-trigger-algorithms/l1/newchod_algorithm/config/NewCHOD.conf");
//	ConfFileReader fileName_("/workspace/na62-trigger-algorithms/l1/newchod_algorithm/config/NewCHOD.2017.conf");
//	ConfFileReader fileName_("/workspace/na62-trigger-algorithms/l1/newchod_algorithm/config/NewCHOD.2017.om.conf");
	//TriggerFile_ = na62::TriggerOptions::GetString(OPTION_NEWCHOD_CONFIG);
	TriggerFile_ = "/etc/na62-trigger.d/l1/NewCHOD.2017.om.conf";
	ConfFileReader fileName_(TriggerFile_);

	if (!fileName_.isValid())
		LOG_ERROR("NewCHOD Config file not found");

	if (fileName_.isValid()) {

		LOG_INFO("NewCHOD configuration file open");

		while (fileName_.nextLine()) {

			if (fileName_.getField<string>(1) == "#") {
				continue;
			}
			if (fileName_.getField<string>(1) == "NROChannels=") {
				nroChannels = fileName_.getField<int>(2);
//				LOG_INFO("nroChannels " << nroChannels);
			}
			if (fileName_.getField<string>(1).find("ChRemap_")
					!= string::npos) {
				for (int iCh = 0; iCh < nroChannels / 16; ++iCh) {
					char name[1000];
					sprintf(name, "ChRemap_%04d=", iCh);
					string remap = (string) name;
//					LOG_INFO(remap);
//					LOG_INFO(fileName_.getField<string>(1));

					if (fileName_.getField<string>(1) == remap) {
						for (int jCh = 0; jCh < 16; jCh++) {
							geoPMTMap[iCh * 16 + jCh] =
									fileName_.getField<int>(jCh + 2);
//							LOG_INFO("geoPMTMap= " << geoPMTMap[iCh * 16 + jCh]);
						}
					}
				}
			}
		}
	}
}

NewCHODParsConfFile::~NewCHODParsConfFile() {
}

NewCHODParsConfFile* NewCHODParsConfFile::GetInstance() {

	if (theInstance == nullptr) {
		theInstance = new NewCHODParsConfFile();
	}
	return theInstance;

}

int* NewCHODParsConfFile::getGeoPMTMap() {
	return geoPMTMap;
}
