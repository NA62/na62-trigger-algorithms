/*
 * ParseConfFile.cpp
 *
 *  Created on: 20 Apr 2015
 *      Author: vfascian
 *  Modified on: 11 May 2015
 *      Author: romano
 */

#include "../../common/ConfFileReader.h"
#include "ParsConfFile.h"

#include <iostream>
#include <vector>
#include <options/Logging.h>
#include <string.h>

#include <sys/time.h>

MUV3ParsConfFile* MUV3ParsConfFile::theInstance = nullptr;

MUV3ParsConfFile::MUV3ParsConfFile() {

//	LOG_INFO("In MUV3 ParseConfFile");

//	ConfFileReader fileName_("/workspace/na62-trigger-algorithms/l1/muv_algorithm/config/MUV3.2016.conf");
//	ConfFileReader fileName_("/workspace/na62-trigger-algorithms/l1/muv_algorithm/config/MUV3.2017.conf");
	ConfFileReader fileName_("/workspace/na62-trigger-algorithms/l1/muv_algorithm/config/MUV3.2017.om.conf");

	if (!fileName_.isValid())
		LOG_ERROR("MUV3 Config file not found");

	if (fileName_.isValid()) {

		LOG_INFO("MUV3 configuration file open");

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
					//LOG_INFO << remap << ENDL;
					//LOG_INFO << fileName_.getField<string>(1) << ENDL;

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

MUV3ParsConfFile::~MUV3ParsConfFile() {
}

MUV3ParsConfFile* MUV3ParsConfFile::GetInstance() {

	if (theInstance == nullptr) {
		theInstance = new MUV3ParsConfFile();
	}
	return theInstance;

}

int* MUV3ParsConfFile::getGeoPMTMap() {
	return geoPMTMap;
}
