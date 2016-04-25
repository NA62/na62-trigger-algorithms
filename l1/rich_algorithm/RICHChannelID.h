/*
 * RICHChannelID.h
 *
 *  Created on: 21 Apr 2015
 *      Author: vfascian
 */

#ifndef L1_RICH_ALGORITHM_RICHCHANNELID_H_
#define L1_RICH_ALGORITHM_RICHCHANNELID_H_

class RICHChannelID{

public:
	RICHChannelID(int geoID);
	~RICHChannelID();

	int getDiskID(int geoID);
	int getUpDownDiskID(int geoID);
	int getSuperCellID(int geoID);
	int getOrSuperCellID(int geoID);
	int getPmtID(int geoID);
	int getChannelSeqID();


private:
	int pmtID;
	int diskID;
	int upDownDiskID;
	int superCellID;
	int orSuperCellID;
};




#endif /* L1_RICH_ALGORITHM_RICHCHANNELID_H_ */
