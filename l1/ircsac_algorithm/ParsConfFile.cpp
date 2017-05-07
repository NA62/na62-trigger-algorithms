/*
 * ParseConfFile.cpp

 *
 *  Created on: 20 May 2016
 *      Author: lorenzaiacobuzio
 */

#include "../../common/ConfFileReader.h"
#include "ParsConfFile.h"

#include <iostream>
#include <vector>
#include <options/Logging.h>
#include <string.h>

#include <sys/time.h>

IRCSACParsConfFile* IRCSACParsConfFile::theInstance = nullptr;

IRCSACParsConfFile::IRCSACParsConfFile() {

//	LOG_INFO("In IRCSAC ParseConfFile");

	ConfFileReader fileName_("/workspace/na62-trigger-algorithms/l1/ircsac_algorithm/config/IRCSAC.conf");

	if (!fileName_.isValid())
		LOG_INFO("IRC SAC Config file not found");

	if (fileName_.isValid()) {

		LOG_INFO("IRC_SAC configuration file open");

		while (fileName_.nextLine()) {

			if (fileName_.getField<string>(1) == "#") {
				continue;
			}
			if (fileName_.getField<string>(1) == "NROChannels=") {
				nroChannels = fileName_.getField<int>(2);
//				LOG_INFO << "nroChannels " << nroChannels << ENDL;
			}
			if (fileName_.getField<string>(1).find("ChRemap_") != string::npos) {
				for (int iCh = 0; iCh < nroChannels / 16; ++iCh) {
					char name[1000];
					sprintf(name, "ChRemap_%04d=", iCh);
					string remap = (string) name;
					//LOG_INFO << remap << ENDL;
					//LOG_INFO << fileName_.getField<string>(1) << ENDL;

					if (fileName_.getField<string>(1) == remap) {
						for (int jCh = 0; jCh < 16; jCh++) {
							geoPMTMap[iCh * 16 + jCh] = fileName_.getField<int>(jCh + 2);
							//LOG_INFO << "geoPMTMap= " << geoPMTMap[iCh * 16 + jCh]<<ENDL;
						}
					}
				}
			}
		}
	}
}

IRCSACParsConfFile::~IRCSACParsConfFile() {
}

IRCSACParsConfFile* IRCSACParsConfFile::GetInstance() {

	if (theInstance == nullptr) {
		theInstance = new IRCSACParsConfFile();
	}
	return theInstance;

}

int* IRCSACParsConfFile::getGeoLGMap() {
	return geoPMTMap;
}

