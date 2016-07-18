/*
 * ParsConfFile.cpp
 *
 *  Created on: 7 Sep 2015
 *      Author: romano
 */

#include "../../common/ConfFileReader.h"
#include "ParsConfFile.h"

#include <iostream>
#include <vector>
#include <options/Logging.h>
#include <string.h>

#include <sys/time.h>

LAVParsConfFile* LAVParsConfFile::theInstance = nullptr;

LAVParsConfFile::LAVParsConfFile() {

//	LOG_INFO("In LAV ParseConfFile");

	ConfFileReader fileName_(
			"/workspace/na62-trigger-algorithms/l1/lav_algorithm/config/LAV.conf");

	if (!fileName_.isValid())
		LOG_INFO("LAV Config file not found");

	if (fileName_.isValid()) {

		LOG_INFO("LAV configuration file open");

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
					sprintf(name, "ChRemap_%d=", iCh);
					string remap = (string) name;
//					LOG_INFO(remap);
//					LOG_INFO(fileName_.getField<string>(1));

					if (fileName_.getField<string>(1) == remap) {
						for (int jCh = 0; jCh < 16; jCh++) {
							geoLGMap[iCh * 16 + jCh] =
									fileName_.getField<int>(jCh + 2);
						}
					}
				}
			}
		}
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


