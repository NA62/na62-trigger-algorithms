/*
 * HitCoordinates.cpp
 *
 *  Created on: Aug 4, 2014
 * 	Author: Thomas Hancock (Thomas.hancock.2012@my.bristol.ac.uk)
 *
 * 	Please Note: Thomas was a summer student who finished on the 12/09/14. 
 * 	He is no longer working on the code, however can still answer questions.
 */

#include "HitCoordinates.h"

namespace na62 {
// Public Functions:
HitCoordinates::HitCoordinates() {
	setCoordinates(0, 0, 0, 0);
}

HitCoordinates::HitCoordinates(double arrayOfCoordinates[]) {
	setCoordinates(arrayOfCoordinates);
}

void HitCoordinates::setCoordinates(double x, double y, double v, double u) {
	m_x = x;
	m_y = y;
	m_v = v;
	m_u = u;
	if ((m_x == 0) || (m_y == 0))
		fillMissingCoordinate();
}

void HitCoordinates::setCoordinates(double arrayOfCoordinates[]) {
	setCoordinates(arrayOfCoordinates[0], arrayOfCoordinates[1],
			arrayOfCoordinates[2], arrayOfCoordinates[3]);
}

void HitCoordinates::printCoordinates() {
	std::cout << "x: " << m_x << std::endl;
	std::cout << "y: " << m_y << std::endl;
	std::cout << "v: " << m_v << std::endl;
	std::cout << "u: " << m_u << std::endl;
}

// Private Functions
void HitCoordinates::fillMissingCoordinate() {
	if ((m_x == 0) && (m_v != 0)) {
		m_x = M_SQRT2 * m_v - m_y;
	} else if ((m_x == 0) && (m_u != 0)) {
		m_x = M_SQRT2 * m_u + m_y;
	} else if ((m_y == 0) && (m_v != 0)) {
		m_y = M_SQRT2 * m_v - m_x;
	} else {
		m_y = m_x - M_SQRT2 * m_u;
	}
}

} /* namespace na62 */

