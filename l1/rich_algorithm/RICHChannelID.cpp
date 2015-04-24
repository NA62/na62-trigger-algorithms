/*
 * RICHChannelID.cpp
 *
 *  Created on: 21 Apr 2015
 *      Author: vfascian
 */

#include <stdlib.h>

#include "RICHChannelID.h"

RICHChannelID::RICHChannelID(int geoID) {

	diskID = geoID / 100000;
	upDownDiskID = (geoID % 100000) / 10000;
	superCellID = (geoID % 10000) / 100;
	orSuperCellID = (geoID % 100) / 10;
	pmtID = geoID % 10;

}

RICHChannelID::~RICHChannelID() {
}

int RICHChannelID::getDiskID(int geoID) {
	return diskID;
}

int RICHChannelID::getUpDownDiskID(int geoID) {
	return upDownDiskID;
}

int RICHChannelID::getSuperCellID(int geoID) {
	return superCellID;
}

int RICHChannelID::getOrSuperCellID(int geoID) {
	return orSuperCellID;
}

int RICHChannelID::getPmtID(int geoID) {
	return pmtID;
}

int RICHChannelID::getChannelSeqID() {

	if (orSuperCellID<1) {
		return superCellID * 8 + pmtID + upDownDiskID * 61 * 8
				+ diskID * 61 * 8 * 2;
	}
	//else{return 61*8*2*2+superCellID+upDownDiskID*61+diskID*61*2;}
	else {return -1;}
}

