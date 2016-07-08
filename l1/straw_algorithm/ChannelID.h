/*
 * ChannelID.h
 *
 *  Created on: 24 Feb 2016
 *      Author: romano
 */

#ifndef L1_STRAW_ALGORITHM_CHANNELID_H_
#define L1_STRAW_ALGORITHM_CHANNELID_H_

namespace na62 {

class STRAWChannelID{

public:
	STRAWChannelID();
	virtual ~STRAWChannelID();
    int encodeChannelID();
	void decodeChannelID(int);
	void resetChannelID();

	int getStrawID();
	int getPlaneID();
	int getHalfViewID();
	int getViewID();
	int getChamberID();


private:
	int strawID;
	int planeID;
	int halfViewID;
	int viewID;
	int chamberID;
};


} /* namespace na62 */

#endif /* L1_STRAW_ALGORITHM_CHANNELID_H_ */
