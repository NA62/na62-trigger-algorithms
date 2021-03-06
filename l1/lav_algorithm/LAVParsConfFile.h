/*
 * ParsConfFile.h
 *
 *  Created on: 7 Sep 2015
 *      Author: romano
 */

#ifndef L1_LAV_ALGORITHM_LAVPARSCONFFILE_H_
#define L1_LAV_ALGORITHM_LAVPARSCONFFILE_H_

class LAVParsConfFile {
public:
       LAVParsConfFile();
       void loadConfigFile(std::string absolute_file_path);

       ~LAVParsConfFile();

       static LAVParsConfFile* GetInstance();

       int* getGeoLGMap();

private:

       static LAVParsConfFile* theInstance;  // singleton instance

       int nroChannels;
       int geoLGMap[6144];

};

#endif /* L1_LAV_ALGORITHM_LAVPARSCONFFILE_H_ */
