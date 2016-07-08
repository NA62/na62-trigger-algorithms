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
	static int* getGeoMap();
	static int getNROChannels();
	static void readT0();
	static double* getT0();

private:

	static STRAWParsConfFile* theInstance;  // singleton instance
	static int nROChannels;
	static int geoMap[8192];
	static string fileT0;
	static double fROMezzaninesT0[512]; //from offline - to be removed

};

#endif /* L1_STRAW_ALGORITHM_PARSCONFFILE_H_ */
