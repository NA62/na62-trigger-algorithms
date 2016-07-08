/*
 * ParsConfFile.h
 *
 *  Created on: 12 may 2016
 *      Author: liacobuz
 */

#ifndef L1_NEW_CHOD_ALGORITHM_PARSCONFFILE_H_
#define L1_NEW_CHOD_ALGORITHM_PARSCONFFILE_H_

#include "../../common/ConfFileReader.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class NewCHODParsConfFile {
public:
	NewCHODParsConfFile();
       ~NewCHODParsConfFile();

       static NewCHODParsConfFile* GetInstance();

       int* getGeoPMTMap();

private:

       static NewCHODParsConfFile* theInstance;  // singleton instance

       int nroChannels;
       int geoPMTMap[512];
};

#endif /* L1_NewCHOD_ALGORITHM_PARSCONFFILE_H_ */
