/*
 * HitCoordinates.h
 *
 *  Created on: Aug 4, 2014
 *      Author: Thomas Hancock (Thomas.hancock@cern.ch)
 */

#pragma once
#ifndef HITCOORDINATES_H_
#define HITCOORDINATES_H_

#include <iostream>
#include <cmath>

//#include "StrawDetectorParameters.h"

namespace na62 {

class HitCoordinates {
public:
	HitCoordinates();
	HitCoordinates(double arrayOfCoordinates[]);
	inline double getX() {return m_x;}
	inline double getY() {return m_y;}
	inline double getV() {return m_v;}
	inline double getU() {return m_u;}
	void setCoordinates(double x, double y, double v, double u);
	void setCoordinates(double arrayOfCoordinates[]);
	void printCoordinates();
	
private:
	void fillMissingCoordinate();
	double m_x;
	double m_y;
	double m_v;
	double m_u;
};

} /* namespace na62 */

#endif /* HITCOORDINATES_H_ */