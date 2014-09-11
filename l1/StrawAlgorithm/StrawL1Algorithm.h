/*
 * StrawL1Algorithm.h
 *
 *  Created on: Aug 7, 2014
 *	Author: Thomas Hancock (Thomas.hancock.2012@my.bristol.ac.uk)
 *	Modified: Laura Rogers (lr12078@my.bristol.ac.uk)
 *
 *	Please Note: Thomas and Laura were both summer students, who finished on the 
 *	12/09/14. They are no longer working on the code, however, can still answer questions.
 */

#pragma once
#ifndef STRAWL1ALGORITHM_H_
#define STRAWL1ALGORITHM_H_

#include <sys/types.h>
#include <cstdint>
#include <iostream>
#include <cstring> // Allows the use of memcpy#include <bitset> // Allows the bitset type to be used#include <cmath> // Allows the use of the pow function#include <vector>#include "StrawData.h"
#include "../HitCoordinates.h"
#include "../ParticlePath.h"
#include "StrawViewVectors.h"
#include "StrawDetectorParameters.h"
#include <eventBuilding/Event.h>
#include <l0/MEPFragment.h>
#include <l0/Subevent.h>

namespace na62 {

class StrawL1Algorithm {
public:
	StrawL1Algorithm(Event* event);
	void compute();

private:
// Functions for locating and getting information about the data
	inline void loadSrb(int srbNo) {
		m_MEPFragment = m_straw->getFragment(srbNo);
	}
	inline void loadData() {
		const l0::MEPFragment_HDR* data = m_MEPFragment->getData();
		m_hitData = ((char*) data) + sizeof(l0::MEPFragment_HDR);
	}
	void loadSrbAndData(int srbNo);
	inline int getNumberOfSrbs() {
		return m_straw->getNumberOfFragments();
	}
	inline uint getDataSize() {
		if (m_MEPFragment == NULL)
			return 0;
		return m_MEPFragment->getDataLength();
	}
// Other functions
	void findHitsInChamber(int chamberNo);
	void printPartBinaries();
	void printBothHitCoordinates();
	double distanceFromXView(int i);
	double findLength(double i, double j);
	double findMissingCoordinate(double m_x, double m_y, double m_v,
			double m_u);

// Data location variables
	l0::Subevent* m_straw;
	l0::MEPFragment* m_MEPFragment;
	char* m_hitData;
// Other variables
	HitCoordinates m_chamberHit[2];
	ParticlePath m_particlePath;
};

} /* namespace na62 */

#endif /* STRAWL1ALGORITHM_H_ */
