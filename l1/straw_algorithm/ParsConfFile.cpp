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
int STRAWParsConfFile::nROChannels = 0;
int STRAWParsConfFile::geoMap[8192];
string STRAWParsConfFile::fileT0 = " ";
double STRAWParsConfFile::fROMezzaninesT0[512];

STRAWParsConfFile::STRAWParsConfFile() {

//	LOG_INFO<< "In STRAW ParsConfFile" << ENDL;

	ConfFileReader fileName_(
			"/Users/romano/Desktop/workspace/na62-trigger-algorithms/l1/straw_algorithm/config/Spectrometer_online.2015.conf");

	int maxChannelID = 0;
	int loopChannels = 0;
	fileT0 = " ";

	if (!fileName_.isValid()) LOG_INFO<< "STRAW Configuration file not found" << ENDL;

	if (fileName_.isValid()) {
		LOG_INFO<< "STRAW configuration file open" << ENDL;

		while (fileName_.nextLine()) {

			if (fileName_.getField<string>(1) == "#") {
				continue;
			}
			if (fileName_.getField<string>(1) == "NROChannels=") {
				nROChannels = fileName_.getField<int>(2);
				if(nROChannels) loopChannels = nROChannels/16;
				else LOG_INFO << "Number of STRAW Readout Channels is empty" << ENDL;
//				LOG_INFO << "Number of (Straw) Readout Channels " << nROChannels << ENDL;
			}
			if (fileName_.getField<string>(1).find("ChRemap_")
					!= string::npos) {
				for (int iCh = 0; iCh != loopChannels; ++iCh) {
					char name[1000];
					sprintf(name, "ChRemap_%04d=", iCh);
					string remap = (string) name;
//					LOG_INFO << remap << ENDL;
//					LOG_INFO << fileName_.getField<string>(1) << ENDL;

					if (fileName_.getField<string>(1) == remap) {
						for (int jCh = 0; jCh != 16; jCh++) {
							geoMap[iCh*16 + jCh] = fileName_.getField<int>(jCh + 2);
							if(geoMap[16*iCh+jCh]>maxChannelID) maxChannelID = geoMap[16*iCh+jCh];
//							LOG_INFO << "(Straw) geoMap " << geoStrawMap[16*iCh+jCh] << ENDL;
						}
					}
				}
			}
			if (fileName_.getField<string>(1).find("ROMezzaninesT0FileInput=")
					!= string::npos) {
				fileT0 = fileName_.getField<string>(2);
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

//	LOG_INFO<< "In STRAW ParsConfFile - T0 Mezzanines file " << fileT0 << ENDL;

	ConfFileReader fileT0_(
			"/Users/romano/Desktop/workspace/na62-trigger-algorithms/l1/straw_algorithm/" + fileT0);

	if (!fileT0_.isValid())
	LOG_INFO<< "STRAW T0 file not found" << ENDL;

	int nROMezzanines = 512;
	int nGroups = nROMezzanines/16;
	if(nROMezzanines%16) nGroups++;

	for(int iGroup=0;iGroup<nGroups;iGroup++){
		for (int iMezzanine=0;iMezzanine<16;iMezzanine++) {
			if(16*iGroup+iMezzanine<nROMezzanines) fROMezzaninesT0[16*iGroup+iMezzanine] = 0.; //set mezzanine T0s to 0
		}
	}
	if (fileT0_.isValid()) {
		LOG_INFO<< "STRAW T0 file " << fileT0 << " open" <<ENDL;

		while (fileT0_.nextLine()) {

			if (fileT0_.getField<string>(1) == "#") {
				continue;
			}
			else if(fileT0_.getField<string>(1).find("MezzaninesT0_") != string::npos) {
				for(int iGroup=0;iGroup<nGroups;iGroup++) {
					char name[1000];
					sprintf(name, "MezzaninesT0_%02d=", iGroup);
					string time = (string) name;
//					LOG_INFO << time << ENDL;
//					LOG_INFO << fileT0_.getField<string>(1) << ENDL;

					if(fileT0_.getField<string>(1) == time) {
//					if(fileT0_.getField<string>(1).find(Form("MezzaninesT0_%02d=",iGroup))!= string::npos) {
						for (int iMezzanine=0;iMezzanine<16;iMezzanine++) {
							if(16*iGroup+iMezzanine<nROMezzanines) {
								fROMezzaninesT0[16*iGroup+iMezzanine] = fileT0_.getField<double>(iMezzanine + 2);
//								LOG_INFO << "(Straw) fROMezzaninesT0 " << fROMezzaninesT0[16*iGroup+iMezzanine] << ENDL;
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
