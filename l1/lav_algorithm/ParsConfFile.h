/*
 * ParsConfFile.h
 *
 *  Created on: 7 Sep 2015
 *      Author: romano
 */

#ifndef L1_LAV_ALGORITHM_PARSCONFFILE_H_
#define L1_LAV_ALGORITHM_PARSCONFFILE_H_

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
       int geoLGMap[6144];

};

#endif /* L1_LAV_ALGORITHM_PARSCONFFILE_H_ */
