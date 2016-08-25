/*
 * ParsConfFile.cpp
 *
 *  Created on: 20 Apr 2015
 *      Author: vfascian
 */

#include "../../common/ConfFileReader.h"
#include "ParsConfFile.h"

#include <options/Logging.h>
#include <iostream>
#include <vector>
#include <string.h>

STRAWParsConfFile* STRAWParsConfFile::theInstance = nullptr;

STRAWParsConfFile::STRAWParsConfFile() {

//	LOG_INFO("In STRAW ParsConfFile");

	ConfFileReader fileName_("/workspace/na62-trigger-algorithms/l1/straw_algorithm/config/Spectrometer_online.2015.conf");

	int maxChannelID = 0;
	int loopChannels = 0;
	fileT0 = " ";

	if (!fileName_.isValid())
		LOG_INFO("STRAW Configuration file not found");

	if (fileName_.isValid()) {
		LOG_INFO("STRAW configuration file open");

		while (fileName_.nextLine()) {

			if (fileName_.getField<string>(1) == "#") {
				continue;
			}
			if (fileName_.getField<string>(1) == "NROChannels=") {
				nROChannels = fileName_.getField<int>(2);
				if (nROChannels)
					loopChannels = nROChannels / 16;
				else
					LOG_INFO("Number of STRAW Readout Channels is empty");
//				LOG_INFO("Number of (Straw) Readout Channels " << nROChannels);
			}
			if (fileName_.getField<string>(1).find("ChRemap_") != string::npos) {
				for (int iCh = 0; iCh != loopChannels; ++iCh) {
					char name[1000];
					sprintf(name, "ChRemap_%04d=", iCh);
					string remap = (string) name;
//					LOG_INFO(remap);
//					LOG_INFO(fileName_.getField<string>(1));

					if (fileName_.getField<string>(1) == remap) {
						for (int jCh = 0; jCh != 16; jCh++) {
							geoMap[iCh * 16 + jCh] = fileName_.getField<int>(jCh + 2);
							if (geoMap[16 * iCh + jCh] > maxChannelID)
								maxChannelID = geoMap[16 * iCh + jCh];
//							LOG_INFO("(Straw) geoMap " << geoStrawMap[16*iCh+jCh]);
						}
					}
				}
			}
			if (fileName_.getField<string>(1).find("ROMezzaninesT0FileInput=") != string::npos) {
				LOG_INFO("Selecting R0MezzaninesT0File " << fileName_.getField<string>(2));
				fileT0 = fileName_.getField<string>(2);
			}
			if (fileName_.getField<string>(1).find("StationsT0FileInput=") != string::npos) {
				LOG_INFO("Selecting StationsT0File " << fileName_.getField<string>(2));
				fileStationT0 = fileName_.getField<string>(2);
			}
		}
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

void STRAWParsConfFile::readT0() {

//	LOG_INFO("In STRAW ParsConfFile - T0 Mezzanines file " << fileT0);

	ConfFileReader fileT0_("/workspace/na62-trigger-algorithms/l1/straw_algorithm/" + fileT0);

	if (!fileT0_.isValid())
		LOG_INFO("STRAW ROMezzanines T0 file not found");

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
		LOG_INFO("STRAW T0 file " << fileT0 << " open");

		while (fileT0_.nextLine()) {

			if (fileT0_.getField<string>(1) == "#") {
				continue;
			} else if (fileT0_.getField<string>(1).find("MezzaninesT0_") != string::npos) {
				for (int iGroup = 0; iGroup < nGroups; iGroup++) {
					char name[1000];
					sprintf(name, "MezzaninesT0_%02d=", iGroup);
					string time = (string) name;
//					LOG_INFO(time);
//					LOG_INFO(fileT0_.getField<string>(1));

					if (fileT0_.getField<string>(1) == time) {
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
	readT0();
	return fROMezzaninesT0;
}

void STRAWParsConfFile::readStationT0() {

	//	LOG_INFO("In STRAW ParsConfFile - Station T0 file " << fileStationT0);

	ConfFileReader fileStationT0_("/workspace/na62-trigger-algorithms/l1/straw_algorithm/" + fileStationT0);

	if (!fileStationT0_.isValid())
		LOG_INFO("STRAW Station T0 file not found");

	if (fileStationT0_.isValid()) {
		LOG_INFO("STRAW Station T0 file " << fileStationT0 << " open");

		while (fileStationT0_.nextLine()) {

			if (fileStationT0_.getField<string>(1) == "#") {
				continue;
			} else if (fileStationT0_.getField<string>(1).find("StationsT0=") != string::npos) {
//				LOG_INFO("(Straw) fStationT0 " << fileStationT0_.getField<double>(2));
				fStationT0 = fileStationT0_.getField<double>(2);
			}
		}
	}
}

double STRAWParsConfFile::getStationT0() {
	readStationT0();
	return fStationT0;
}
