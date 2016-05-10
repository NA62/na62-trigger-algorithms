/*
 * ChannelID.cpp
 *
 *  Created on: 24 Feb 2016
 *      Author: romano
 */

#include "ChannelID.h"

#include <stdlib.h>


namespace na62 {

STRAWChannelID::STRAWChannelID() {
	strawID = -1;
	planeID = -1;
	halfViewID = -1;
	viewID = -1;
	chamberID = -1;
}

STRAWChannelID::~STRAWChannelID() {
}

int STRAWChannelID::encodeChannelID() {
	return (chamberID*16 + viewID*4 + halfViewID*2 + planeID)*122 + strawID;
}

void STRAWChannelID::decodeChannelID(int chID) {
	chamberID = chID/1952;
	viewID = (chID%1952)/488;
	halfViewID = (chID%488)/244;
	planeID = (chID%244)/122;
	strawID = chID%122;
}

void STRAWChannelID::resetChannelID() {
	strawID = -1;
	planeID = -1;
	halfViewID = -1;
	viewID = -1;
	chamberID = -1;
}
int STRAWChannelID::getStrawID() {
		return strawID;
}

int STRAWChannelID::getPlaneID() {
	return planeID;
}

int STRAWChannelID::getHalfViewID() {
	return halfViewID;
}

int STRAWChannelID::getViewID() {
	return viewID;
}

int STRAWChannelID::getChamberID() {
	return chamberID;
}

} /* namespace na62 */
