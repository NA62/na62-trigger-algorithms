/*
 * ParsConfFile.cpp
 *
 *  Created on: 7 Sep 2015
 *      Author: romano
 */


#include <string>

#include <options/Logging.h>
#include <common/ConfFileReader.h>
#include <l1/ConfPath.h>
#include "LAVParsConfFile.h"

LAVParsConfFile* LAVParsConfFile::theInstance = nullptr;

LAVParsConfFile::LAVParsConfFile() {
}

void LAVParsConfFile::loadConfigFile(std::string absolute_file_path) {
	//ConfFileReader fileName_(LAV_CONFIG_FILE);
	ConfFileReader fileName_(absolute_file_path);


	if (fileName_.isValid()) {
		LOG_INFO("LAV configuration file open");

		while (fileName_.nextLine()) {
			if (fileName_.getField<std::string>(1) == "#") {
				continue;
			}
			if (fileName_.getField<std::string>(1) == "NROChannels=") {
				nroChannels = fileName_.getField<int>(2);
//				LOG_INFO("nroChannels " << nroChannels);
			}
			if (fileName_.getField<std::string>(1).find("ChRemap_")
					!= std::string::npos) {
				for (int iCh = 0; iCh < nroChannels / 16; ++iCh) {
					char name[1000];
					sprintf(name, "ChRemap_%04d=", iCh);
					std::string remap = (std::string) name;
//					LOG_INFO(remap);
//					LOG_INFO(fileName_.getField<string>(1));

					if (fileName_.getField<std::string>(1) == remap) {
						for (int jCh = 0; jCh < 16; jCh++) {
							geoLGMap[iCh * 16 + jCh] =
									fileName_.getField<int>(jCh + 2);
//							LOG_INFO("geoLGMap " << geoLGMap[iCh * 16 + jCh]);
						}
					}
				}
			}
		}
	} else {
		LOG_ERROR("LAV Config file not found");
	}

}

LAVParsConfFile::~LAVParsConfFile() {
}

LAVParsConfFile* LAVParsConfFile::GetInstance() {

	if (theInstance == nullptr) {
		theInstance = new LAVParsConfFile();
	}
	return theInstance;

}

int* LAVParsConfFile::getGeoLGMap() {
	return geoLGMap;
}


