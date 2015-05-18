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

ParsConfFile* ParsConfFile::theInstance = nullptr;

ParsConfFile::ParsConfFile() {

//	LOG_INFO<< "In ParseConfFile" << ENDL;

	ConfFileReader fileName_(
			"/Users/romano/Desktop/workspace/na62-trigger-algorithms/l1/chod_algorithm/config/CHOD.conf");

	if (!fileName_.isValid())
		LOG_INFO<< "Config file not found" << ENDL;

	if (fileName_.isValid()) {

//		LOG_INFO<< "File CHOD.conf open" << ENDL;

		while (fileName_.nextLine()) {

			if (fileName_.getField<string>(1) == "#") {
				continue;
			}
			if (fileName_.getField<string>(1) == "NROChannels=") {
				nroChannels = fileName_.getField<int>(2);
//				LOG_INFO << "nroChannels " << nroChannels << ENDL;
			}
			if (fileName_.getField<string>(1).find("ChRemap_")
					!= string::npos) {
				for (int iCh = 0; iCh < nroChannels / 16; ++iCh) {
					char name[1000];
					sprintf(name, "ChRemap_%d=", iCh);
					string remap = (string) name;
//					LOG_INFO << remap << ENDL;
//					LOG_INFO << fileName_getField<string>(1) << ENDL;

					if (fileName_.getField<string>(1) == remap) {
						for (int jCh = 0; jCh < 16; jCh++) {
							geoSlabMap[iCh * 16 + jCh] =
									fileName_.getField<int>(jCh + 2);
						}
					}
				}
			}
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

int* ParsConfFile::getGeoSlabMap() {
	return geoSlabMap;
}

//void ParsConfFile::readT0() {
//
////     LOG_INFO<< "ParsFile::File T0 " << fileT0 << ENDL;
//       ConfFileReader fileT0_("");
//
//       if (!fileT0_.isValid())
////     LOG_INFO<< "T0 file not found" << ENDL;
//
//               if (fileT0_.isValid()) {
//
////             LOG_INFO<< "T0 file " << fileT0 << " open" <<ENDL;
//
//                       while (fileT0_.nextLine()) {
//
//                               if (fileT0_.getField<string>(1) == "#") {
//                                       continue;
//                               }
//
//                               if (fileT0_.getField<string>(1).find("T0_") != string::npos) {
//                                       for (int iCh = 0; iCh < nChannels / 16; ++iCh) {
//                                               char name[1000];
//                                               sprintf(name, "T0_%d=", iCh);
//                                               string time = (string) name;
//                                               //LOG_INFO << remap << ENDL;
//                                               //LOG_INFO << fileName.getField<string>(1) << ENDL;
//
//                                               if (fileT0_.getField<string>(1) == time) {
//                                                       for (int jCh = 0; jCh < 16; jCh++) {
//                                                               timeT0[iCh * 16 + jCh] =
//                                                                               fileT0_.getField<double>(jCh + 2);
//                                                       }
//                                               }
//                                       }
//                               }
//                       }
//               }
//}
//
//double* ParsConfFile::getT0() {
//       readT0();
//       return timeT0;
//}
