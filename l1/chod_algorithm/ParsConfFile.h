/*
 * ParsConfFile.h
 *
 *  Created on: 20 Apr 2015
 *      Author: vfascian
 *  Modified on: 11 May 2015
 *      Author: romano
 */

#ifndef L1_CHOD_ALGORITHM_PARSCONFFILE_H_
#define L1_CHOD_ALGORITHM_PARSCONFFILE_H_

#include "../../common/ConfFileReader.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class CHODParsConfFile {
public:
       CHODParsConfFile();
       ~CHODParsConfFile();

       static CHODParsConfFile* GetInstance();

       int* getGeoSlabMap();
//       double* getT0();
//       void readT0();

private:

       static CHODParsConfFile* theInstance;  // singleton instance

       int nroChannels;
       int geoSlabMap[512];
//       double timeT0[512];
//       string fileT0;

};

#endif /* L1_CHOD_ALGORITHM_PARSCONFFILE_H_ */
