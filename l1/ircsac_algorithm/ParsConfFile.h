/*
 * ParseConfFile.h
 *
 *  Created on: 20 May 2016
 *      Author: lorenza
 */

#ifndef L1_IRCSAC_ALGORITHM_PARSCONFFILE_H_
#define L1_IRCSAC_ALGORITHM_PARSCONFFILE_H_

#include "../../common/ConfFileReader.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class IRCSACParsConfFile {
public:
		IRCSACParsConfFile();
       ~IRCSACParsConfFile();

       static IRCSACParsConfFile* GetInstance();

       int* getGeoLGMap();

private:

       static IRCSACParsConfFile* theInstance;  // singleton instance

       int nroChannels;
       int geoPMTMap[512];
};

#endif /* L1_IRCSAC_ALGORITHM_PARSCONFFILE_H_ */
