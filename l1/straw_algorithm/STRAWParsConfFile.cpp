/*
 * ParsConfFile.cpp
 *
 *  Created on: 20 Apr 2015
 *      Author: vfascian
 */

#include "STRAWParsConfFile.h"

#include <common/ConfFileReader.h>
#include <l1/ConfPath.h>

#include <options/Logging.h>
#include <string>
#include <iostream>

STRAWParsConfFile* STRAWParsConfFile::theInstance = nullptr;

STRAWParsConfFile::STRAWParsConfFile() {


}

void STRAWParsConfFile::loadConfigFile(std::string absolute_file_path) {
	ConfFileReader fileName_(absolute_file_path);
	int maxChannelID = 0;
	int loopChannels = 0;
	fileT0 = " ";

	if (fileName_.isValid()) {
		//LOG_INFO(" > HLT STRAW configuration file opening: " << fileName_.getFilename());

		while (fileName_.nextLine()) {

			if (fileName_.getField<std::string>(1) == "#") {
				continue;
			}
			if (fileName_.getField<std::string>(1) == "NROChannels=") {
				nROChannels = fileName_.getField<int>(2);
				if (nROChannels) {
					loopChannels = nROChannels / 16;
				} else {
					LOG_ERROR("Number of STRAW Readout Channels is empty");
				}
//				LOG_INFO("Number of (Straw) Readout Channels " << nROChannels);
			}
			if (fileName_.getField<std::string>(1).find("ChRemap_") != std::string::npos) {
				for (int iCh = 0; iCh != loopChannels; ++iCh) {
					char name[1000];
					sprintf(name, "ChRemap_%04d=", iCh);
					std::string remap = (std::string) name;
//					LOG_INFO(remap);
//					LOG_INFO(fileName_.getField<string>(1));

					if (fileName_.getField<std::string>(1) == remap) {
						for (int jCh = 0; jCh != 16; jCh++) {
							geoMap[iCh * 16 + jCh] = fileName_.getField<int>(jCh + 2);
							if (geoMap[16 * iCh + jCh] > maxChannelID)
								maxChannelID = geoMap[16 * iCh + jCh];
//							LOG_INFO("(Straw) geoMap " << geoMap[16*iCh+jCh]);
						}
					}
				}
			}
			if (fileName_.getField<std::string>(1).find("ROMezzaninesT0FileInput=") != std::string::npos) {
				fileT0 = fileName_.getField<std::string>(2);
//				LOG_INFO("(Straw) Selecting R0MezzaninesT0File " << fileT0);
			}
			if ((fileName_.getField<std::string>(1).find("StationsT0=") != std::string::npos) && (fileName_.getField<std::string>(1).size() == 11)) {
				fStationT0 = fileName_.getField<double>(2);
//				LOG_INFO("(Straw) fStationT0 " << fStationT0);
			}
			if ((fileName_.getField<std::string>(1).find("MagicT0=") != std::string::npos) && (fileName_.getField<std::string>(1).size() == 8)) {
				fMagicT0 = fileName_.getField<double>(2);
//				LOG_INFO("(Straw) fMagicT0 " << fMagicT0);
			}
		}
	} else {
		LOG_ERROR(" > HLT STRAW Configuration file not found: " << fileName_.getFilename());
	}
}



STRAWParsConfFile::~STRAWParsConfFile() {
}

STRAWParsConfFile* STRAWParsConfFile::GetInstance() {

	if (theInstance == nullptr) {
		theInstance = new STRAWParsConfFile();
	}
	return theInstance;

}

int* STRAWParsConfFile::getGeoMap() {
	return geoMap;
}

int STRAWParsConfFile::getNROChannels() {
	return nROChannels;
}

void STRAWParsConfFile::readT0(std::string absolute_t0_path) {

	ConfFileReader fileT0_(absolute_t0_path + fileT0);

	if (!fileT0_.isValid()) {
		LOG_ERROR("STRAW ROMezzanines T0 file not found");
	}

	int nROMezzanines = 512;
	int nGroups = nROMezzanines / 16;
	if (nROMezzanines % 16)
		nGroups++;

	for (int iGroup = 0; iGroup < nGroups; iGroup++) {
		for (int iMezzanine = 0; iMezzanine < 16; iMezzanine++) {
			if (16 * iGroup + iMezzanine < nROMezzanines)
				fROMezzaninesT0[16 * iGroup + iMezzanine] = 0.; //set mezzanine T0s to 0
		}
	}
	if (fileT0_.isValid()) {
		LOG_INFO("STRAW T0 file " << fileT0_.getFilename() << " open");

		while (fileT0_.nextLine()) {

			if (fileT0_.getField<std::string>(1) == "#") {
				continue;
			} else if (fileT0_.getField<std::string>(1).find("MezzaninesT0_") != std::string::npos) {
				for (int iGroup = 0; iGroup < nGroups; iGroup++) {
					char name[1000];
					sprintf(name, "MezzaninesT0_%02d=", iGroup);
					std::string time = (std::string) name;
//					LOG_INFO(time);
//					LOG_INFO(fileT0_.getField<string>(1));

					if (fileT0_.getField<std::string>(1) == time) {
//					if(fileT0_.getField<string>(1).find(Form("MezzaninesT0_%02d=",iGroup))!= string::npos) {
						for (int iMezzanine = 0; iMezzanine < 16; iMezzanine++) {
							if (16 * iGroup + iMezzanine < nROMezzanines) {
								fROMezzaninesT0[16 * iGroup + iMezzanine] = fileT0_.getField<double>(iMezzanine + 2);
//								LOG_INFO("(Straw) fROMezzaninesT0 " << fROMezzaninesT0[16*iGroup+iMezzanine]);
							}
						}
					}
				}
			}
		}
	}
}

double* STRAWParsConfFile::getT0() {
	return fROMezzaninesT0;
}

double STRAWParsConfFile::getStationT0() {
	return fStationT0;
}

double STRAWParsConfFile::getMagicT0() {
	return fMagicT0;
}
