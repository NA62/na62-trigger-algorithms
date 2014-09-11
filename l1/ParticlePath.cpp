/*
 * ParticlePath.cpp
 *
 *  Created on: Aug 6, 2014
 * 	Author: Thomas Hancock (Thomas.hancock.2012@my.bristol.ac.uk)
 *
 * 	Please Note: Thomas was a summer student who finished on the 12/09/14. 
 * 	He is no longer working on the code, however can still answer questions.
 */ 

#include "ParticlePath.h"

namespace na62 {
// Public Functions
ParticlePath::ParticlePath() {
}

ParticlePath::ParticlePath(HitCoordinates Chamber1Hit, HitCoordinates Chamber2Hit) {
	initialise(Chamber1Hit, Chamber2Hit);
}

ParticlePath::ParticlePath(double xCoor1, double xCoor2, double yCoor1, double yCoor2) {
	initialise(xCoor1, xCoor2, yCoor1, yCoor2);
}

void ParticlePath::printPathParameters() {
	std::cout << "Length: " << m_pathLength << std::endl;
	std::cout << "Angle from Z: " << m_angleFromZAxis << std::endl;
	std::cout << "Angle from Y: " << m_angleFromYAxis << std::endl;
	std::cout << "Coordinate in chamber 1: " << m_xycoorInChamber1[0] << ", " << m_xycoorInChamber1[1] << std::endl;
}

void ParticlePath::printCoordinateDifferences() {
	std::cout << "DeltaX: " << m_deltaX << std::endl;
	std::cout << "DeltaY: " << m_deltaY << std::endl;
	std::cout << "DeltaZ: " << m_deltaZ << std::endl;
}

void ParticlePath::initialise(double xCoor1, double xCoor2, double yCoor1, double yCoor2) {
	m_deltaX = xCoor2 - xCoor1;
	m_deltaY = yCoor2 - yCoor1;
	m_deltaZ = strawparameters::CHAMBERS_ONE_AND_TWO_SPACING;
	m_xycoorInChamber1[0] = xCoor1;
	m_xycoorInChamber1[1] = yCoor1;
	calculatePathParameters();
}

void ParticlePath::initialise(HitCoordinates Chamber1Hit, HitCoordinates Chamber2Hit) {
	initialise(Chamber1Hit.getX(), Chamber2Hit.getX(), Chamber1Hit.getY(), Chamber2Hit.getY());
}

// Private Functions
void ParticlePath::calculatePathLength() {
	m_pathLength = sqrt(m_deltaX*m_deltaX + m_deltaY*m_deltaY + m_deltaZ*m_deltaZ);
}

void ParticlePath::calculateAngleFromZAxis() {
	m_angleFromZAxis = atan(sqrt(m_deltaX*m_deltaX + m_deltaY*m_deltaY)/m_deltaZ);
}

void ParticlePath::calculateAngleFromYAxis() {
	m_angleFromYAxis = atan2(m_deltaX, m_deltaY);
}

void ParticlePath::calculatePathParameters() {
	calculatePathLength();
	calculateAngleFromZAxis();
	calculateAngleFromYAxis();
}

} /* namespace na62 */
