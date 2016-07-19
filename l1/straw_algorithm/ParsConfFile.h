/*
 * ParsConfFile.h
 *
 *  Created on: 20 Apr 2015
 *      Author: vfascian
 */

#ifndef L1_STRAW_ALGORITHM_PARSCONFFILE_H_
#define L1_STRAW_ALGORITHM_PARSCONFFILE_H_

#include "../../common/ConfFileReader.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class STRAWParsConfFile {
public:
	STRAWParsConfFile();
	~STRAWParsConfFile();

	static STRAWParsConfFile* GetInstance();

	int* getGeoMap();
	int getNROChannels();

	void readT0();
	double* getT0();

	void readStationT0();
	double getStationT0();

private:

	static STRAWParsConfFile* theInstance;  // singleton instance

	int nROChannels;
	int geoMap[8192];

	string fileT0;
	double fROMezzaninesT0[512]; //from offline - to be removed

	string fileStationT0;
	double fStationT0;

};

#endif /* L1_STRAW_ALGORITHM_PARSCONFFILE_H_ */
