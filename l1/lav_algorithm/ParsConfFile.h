/*
 * ParsConfFile.h
 *
 *  Created on: 07 Sept 2015
 *      Author: romano
 */

#ifndef L1_LAV_ALGORITHM_PARSCONFFILE_H_
#define L1_LAV_ALGORITHM_PARSCONFFILE_H_

#include "../../common/ConfFileReader.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class LAVParsConfFile {
public:
       LAVParsConfFile();
       ~LAVParsConfFile();

       static LAVParsConfFile* GetInstance();

       int* getGeoLGMap();

private:

       static LAVParsConfFile* theInstance;  // singleton instance

       int nroChannels;
       int geoLGMap[512];

};

#endif /* L1_LAV_ALGORITHM_PARSCONFFILE_H_ */
