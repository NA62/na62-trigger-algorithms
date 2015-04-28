/*
 * ParsConfFile.cpp
 *
 *  Created on: 20 Apr 2015
 *      Author: vfascian
 */

#include "ConfFileReader.h"
#include "ParsConfFile.h"

#include <iostream>
#include <vector>
#include <options/Logging.h>
#include <string.h>

ParsConfFile* ParsConfFile::theInstance = nullptr;

ParsConfFile::ParsConfFile() {

	LOG_INFO<< "In ParseConfFile" << ENDL;

	fileT0 = " ";

	ConfFileReader fileName_("/Users/vfascian/Documents/workspace/na62-trigger-algorithms/l1/rich_algorithm/config/RICH.conf");

	if (!fileName_.isValid()) LOG_INFO << "Config file not found" << ENDL;

	if (fileName_.isValid()) {

		LOG_INFO << "File RICH.conf open" << ENDL;

		while (fileName_.nextLine()) {

			if (fileName_.getField<string>(1) == "#") {
				continue;
			}
			if (fileName_.getField<string>(1) == "NROChannels=") {
				nroChannels = fileName_.getField<int>(2);
				//LOG_INFO << "nChannels " << nChannels << ENDL;
			}

			if (fileName_.getField<string>(1) == "NChannels=") {
				nChannels = fileName_.getField<int>(2);
				//LOG_INFO << "nChannels " << nChannels << ENDL;
			}

			if (fileName_.getField<string>(1) == "JuraRotation=") {
				focalCenterJura[0] = fileName_.getField<int>(2);
				focalCenterJura[1] = fileName_.getField<int>(3);
				//LOG_INFO << "Jura shift " << focalCenterJura[0] << " " << focalCenterJura[1] << ENDL;

			}

			if (fileName_.getField<string>(1) == "SaleveRotation=") {
				focalCenterSaleve[0] = fileName_.getField<int>(2);
				focalCenterSaleve[1] = fileName_.getField<int>(3);
				//LOG_INFO << "Saleve shift " << focalCenterSaleve[0] << " " << focalCenterSaleve[1] << ENDL;

			}

			if (fileName_.getField<string>(1).find("ChRemap_") != string::npos) {
				for (int iCh = 0; iCh < nroChannels / 16; ++iCh) {
					char name[1000];
					sprintf(name, "ChRemap_%d=", iCh);
					string remap = (string) name;
					//LOG_INFO << remap << ENDL;
					//LOG_INFO << fileName.getField<string>(1) << ENDL;

					if (fileName_.getField<string>(1) == remap) {
						for (int jCh = 0; jCh < 16; jCh++) {
							geoPmsMap[iCh*16+jCh] = fileName_.getField<int>(jCh + 2);
						}
					}
				}
			}

			if (fileName_.getField<string>(1).find("PMPosition_SC_") != string::npos) {
				for (int iCh = 0; iCh < nChannels /16; ++iCh) {
					char name[1000];
					sprintf(name, "PMPosition_SC_%d=", iCh);
					string position = (string) name;
					//LOG_INFO << remap << ENDL;
					//LOG_INFO << fileName.getField<string>(1) << ENDL;

					if (fileName_.getField<string>(1) == position) {
						for (int jCh = 0; jCh < 16; ++jCh) {
							//LOG_INFO << "i1 " << iCh*16+jCh << " i1 Pos " << fileName_.getField<double>(jCh + 2) << ENDL;
							//LOG_INFO << "i2 " << iCh*16+jCh + nChannels << " i2 Pos " << fileName.getField<double>(jCh + 2) << ENDL;
							posPmsMap[iCh*16+jCh] = fileName_.getField<double>(jCh + 2);
							//posPmsMap[iCh*16+jCh+nChannels] = fileName.getField<double>(jCh + 2);
						}
					}
				}
			}

			if (fileName_.getField<string>(1).find("T0CorrectionFileInput") != string::npos) {
				fileT0 = fileName_.getField<string>(2);
			}

//			if (fileName.getField<string>(1) == "SCPosition_SC_60=") {
//				LOG_INFO << "End file" << ENDL;
//			}
		}
	}
}

ParsConfFile::~ParsConfFile() {
}

ParsConfFile* ParsConfFile::GetInstance() {

	if (theInstance == nullptr) {
		theInstance = new ParsConfFile();
	}
	return theInstance;

}

//int ParsConfFile::getChannelRemap(int ROID) {
//
//	int geoID = channelRemap.at(ROID);
//	return geoID;
//
//}

int* ParsConfFile::getGeoPmsMap() {
	return geoPmsMap;
}

double* ParsConfFile::getPosPmsMap() {
	return posPmsMap;
}

int* ParsConfFile::getFocalCenterJura() {
	return focalCenterJura;
}

int* ParsConfFile::getFocalCenterSaleve() {
	return focalCenterSaleve;
}

void ParsConfFile::readT0() {

	LOG_INFO<< "ParsFile::File T0 " << fileT0 << ENDL;
	ConfFileReader fileT0_("/Users/vfascian/Documents/workspace/na62-trigger-algorithms/l1/rich_algorithm/" + fileT0);

	if (!fileT0_.isValid())
	LOG_INFO<< "T0 file not found" << ENDL;

	if (fileT0_.isValid()) {

		LOG_INFO<< "T0 file " << fileT0 << " open" <<ENDL;

		while (fileT0_.nextLine()) {

			if (fileT0_.getField<string>(1) == "#") {continue;}

			if (fileT0_.getField<string>(1).find("T0_") != string::npos) {
				for (int iCh = 0; iCh < nChannels / 16; ++iCh) {
					char name[1000];
					sprintf(name, "T0_%d=", iCh);
					string time = (string) name;
					//LOG_INFO << remap << ENDL;
					//LOG_INFO << fileName.getField<string>(1) << ENDL;

					if (fileT0_.getField<string>(1) == time) {
						for (int jCh = 0; jCh < 16; jCh++) {
							timeT0[iCh*16+jCh] = fileT0_.getField<double>(jCh + 2);
						}
					}
				}
			}
		}
	}
}

double* ParsConfFile::getT0() {
	return timeT0;
}

