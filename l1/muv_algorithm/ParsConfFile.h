/*
 * ParsConfFile.h
 *
 *  Created on: 20 Apr 2015
 *      Author: vfascian
 *  Modified on: 11 May 2015
 *      Author: romano
 */

#ifndef L1_MUV3_ALGORITHM_PARSCONFFILE_H_
#define L1_MUV3_ALGORITHM_PARSCONFFILE_H_

#include "../../common/ConfFileReader.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class MUV3ParsConfFile {
public:
       MUV3ParsConfFile();
       ~MUV3ParsConfFile();

       static MUV3ParsConfFile* GetInstance();

       int* getGeoPMTMap();

private:

       static MUV3ParsConfFile* theInstance;  // singleton instance

       int nroChannels;
       int geoPMTMap[512];
       std::string TriggerFile_;
};

#endif /* L1_MUV3_ALGORITHM_PARSCONFFILE_H_ */
