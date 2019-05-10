/*
 * ParsConfFile.h
 *
 *  Created on: 20 Apr 2015
 *      Author: vfascian
 */

#ifndef L1_STRAW_ALGORITHM_STRAWPARSCONFFILE_H_
#define L1_STRAW_ALGORITHM_STRAWPARSCONFFILE_H_

#include <string>

class STRAWParsConfFile {
public:
	STRAWParsConfFile();
	~STRAWParsConfFile();

	static STRAWParsConfFile* GetInstance();
	void loadConfigFile(std::string absolute_file_path);

	int* getGeoMap();
	int getNROChannels();

	void readT0(std::string absolute_t0_path);
	double* getT0();

	void readStationT0();
	double getStationT0();
	double getMagicT0();

private:

	static STRAWParsConfFile* theInstance;  // singleton instance

	int nROChannels;
	int geoMap[8192];

	std::string fileT0;
	double fROMezzaninesT0[512]; //from offline - to be removed

	double fStationT0;
	double fMagicT0;

};

#endif /* L1_STRAW_ALGORITHM_STRAWPARSCONFFILE_H_ */
