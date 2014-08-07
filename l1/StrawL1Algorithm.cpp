/*
 * StrawL1Algorithm.cpp
 *
 *  Created on: Aug 7, 2014
 *      Author: Thomas Hancock (Thomas.hancock@cern.ch)
 */
 
#include "StrawL1Algorithm.h"

namespace na62 {
// Public Functions
StrawL1Algorithm::StrawL1Algorithm(Event* event) {
	m_straw = event->getSTRAWSubevent(); // Access the Straw Detector
	m_MEPFragment = NULL;
	m_hitData = NULL;
}

void StrawL1Algorithm::compute() {
	findHitsInChamber(0);
	findHitsInChamber(1);
	m_particlePath.initialise(m_chamberHit[0], m_chamberHit[1]);

	printBothHitCoordinates();
	std::cout << std::endl << "Path Parameters: " << std::endl;
	m_particlePath.printPathParameters();

	std::cout << "Test 37" << std::endl; // Provides a way to quickly check if the program compiled successfully
}

// Private Functions
void StrawL1Algorithm::loadData(int partNo) {
	getPart(partNo);
	getData();
}

void StrawL1Algorithm::findHitsInChamber(int chamberNo) {
	double hitDisplacementArray[4];
	for (int i = chamberNo*8; i < (chamberNo+1)*8; i+=2) {
		loadData(i);
		StrawData strawData (m_hitData);
		if (strawData.getNumberOfHits() == 0) {
			hitDisplacementArray[(int) floor((i%8)/2.0)] = 0.0;
		} else {
			hitDisplacementArray[(int) floor((i%8)/2.0)] = strawData.getStrawDistance();
		}
	}
	m_chamberHit[chamberNo].setCoordinates(hitDisplacementArray);
}

void StrawL1Algorithm::printPartBinaries() {
	uint dataSize = getDataSize();
	std::bitset<32> binaryLine;
	for (int j = 0; j < (int) floor(dataSize/4.0)-2; j++) {
		std::cout << j << ": ";
		memcpy(&binaryLine, (m_hitData + (j*4)), 4);
		for (int i = 0; i < 32; i++) {
			std::cout << binaryLine[i];
			if ((i % 8) == 7) std::cout << " ";
		}
		std::cout << std::endl;
	}
}

void StrawL1Algorithm::printBothHitCoordinates() {
	std::cout << std::endl << "Hit 1:" << std::endl;
	m_chamberHit[0].printCoordinates();	
	std::cout << std::endl << "Hit 2:" << std::endl;
	m_chamberHit[1].printCoordinates();
}

} /* namespace na62 */