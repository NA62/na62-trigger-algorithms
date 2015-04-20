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

	ConfFileReader fileName("/Users/vfascian/Documents/workspace/na62-trigger-algorithms/l1/rich_algorithm/RICH.conf");

	if (!fileName.isValid()) LOG_INFO << "Config file not found" << ENDL;

	if (fileName.isValid()) {

		LOG_INFO << "File RICH.conf open" << ENDL;

		while (fileName.nextLine()) {

			if (fileName.getField<string>(1) == "#") {
				continue;
			}
			if (fileName.getField<string>(1) == "NROChannels=") {
				nChannels = fileName.getField<int>(2);
				//LOG_INFO << "nChannels " << nChannels << ENDL;
			}

			if (fileName.getField<string>(1).find("ChRemap_") != string::npos) {
				for (int iCh = 0; iCh < nChannels / 16; iCh++) {
					char name[1000];
					sprintf(name, "ChRemap_%d=", iCh);
					string remap = (string) name;
					//LOG_INFO << remap << ENDL;
					//LOG_INFO << fileName.getField<string>(1) << ENDL;

					if (fileName.getField<string>(1) == remap) {
						for (int jCh = 0; jCh < 16; jCh++) {
							geoChannelMap.push_back(fileName.getField<int>(jCh + 2));
						}
					}
				}
			}

			if (fileName.getField<string>(1).find("PMPosition_SC_") != string::npos) {
				for (int iCh = 0; iCh < nChannels /16; iCh++) {
					char name[1000];
					sprintf(name, "PMPosition_SC_%d=", iCh);
					string position = (string) name;
					//LOG_INFO << remap << ENDL;
					//LOG_INFO << fileName.getField<string>(1) << ENDL;

					if (fileName.getField<string>(1) == position) {
						for (int jCh = 0; jCh < 16; jCh++) {
							posChannelMap.push_back(fileName.getField<double>(jCh + 2));
						}
					}
				}
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

vector<int> ParsConfFile::getGeoChannelMap() {
	return geoChannelMap;
}
vector<double> ParsConfFile::getPosChannelMap() {
	return posChannelMap;
}

