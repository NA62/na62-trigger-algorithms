/*
 * ParticlePath.h
 *
 *  Created on: Aug 6, 2014
 *      Author: Thomas Hancock (Thomas.hancock@cern.ch)
 */

#pragma once
#ifndef PARTICLEPATH_H_
#define PARTICLEPATH_H_

#include <iostream>
#include <cmath>

#include "StrawDetectorParameters.h"
#include "HitCoordinates.h"

namespace na62 {

class ParticlePath {
public:
	ParticlePath();
	ParticlePath(HitCoordinates Chamber1Hit, HitCoordinates Chamber2Hit);
	ParticlePath(double xCoor1, double xCoor2, double yCoor1, double yCoor2);
	inline double getPathLength() {return m_pathLength;}
	inline double getPathAngle() {return m_angleFromZAxis;}
	void printPathParameters();
	void printCoordinateDifferences();
	void initialise(double xCoor1, double xCoor2, double yCoor1, double yCoor2);
	void initialise(HitCoordinates Chamber1Hit, HitCoordinates Chamber2Hit);
private:
	void calculatePathLength();
	void calculateAngleFromZAxis();
	void calculateAngleFromYAxis();
	void calculatePathParameters();
	double m_deltaX;
	double m_deltaY;
	double m_deltaZ;
	double m_pathLength;
	double m_angleFromZAxis;
	double m_angleFromYAxis;
	double m_xycoorInChamber1[2];
};

} /* namespace na62 */

#endif /* PARTICLEPATH_H_ */