/*
* StrawL1Algorithm.cpp
*
*  Created on: Aug 7, 2014
*      Author: Thomas Hancock (Thomas.hancock@cern.ch)
*      Modified: Laura Rogers (laura.kathryn.rogers@cern.ch)
*
*      Please Note: Thomas was a summer student who finished on the 15/8/14. While he can still answer questions, he is no longer working on the code.
*/ 


#include "StrawL1Algorithm.h"

namespace na62 {
	// Public Functions
	StrawL1Algorithm::StrawL1Algorithm(Event* event) {
		m_straw = event->getSTRAWSubevent(); // Access the Straw Detector
		m_MEPFragment = NULL;
		m_hitData = NULL;
	}
	
	/*void initialiseViewVectors() {
		std::vector <double> vectorX1(0); //defines eight vectors, one for each view in chamber one and again for chamber 2, X,Y,V,U
		std::vector <double> vectorY1(0);	
		std::vector <double> vectorV1(0);
		std::vector <double> vectorU1(0); 
		std::vector <double> vectorX2(0);
		std::vector <double> vectorY2(0);	
		std::vector <double> vectorV2(0);
		std::vector <double> vectorU2(0); 
		std::cout<<getNumberOfSrbs()<<std::endl; //returns number of Srbs=32
		
		std::vector <std::vector <double> * > vectors(0); //this vector contains all eight previous vectors, added below
				
		vectors.push_back(&vectorX1);
		vectors.push_back(&vectorY1); 
		vectors.push_back(&vectorV1);
		vectors.push_back(&vectorU1);
		vectors.push_back(&vectorX2);
		vectors.push_back(&vectorY2);
		vectors.push_back(&vectorV2);
		vectors.push_back(&vectorU2);
				
		int j;
		for(int i=0;i<16;i++) {
			j=(floor) (i/2);
			loadSrbAndData(i);
			StrawData strawData(m_hitData);		
			vectors[j]->push_back(strawData.getStrawDistance());	
			std::cout<<"For SRB "<<i<<"... "<< (double) strawData.getStrawDistance()<<std::endl;			
		}  
	} */

	void StrawL1Algorithm::compute() {
		findHitsInChamber(0);
		findHitsInChamber(1);
		m_particlePath.initialise(m_chamberHit[0], m_chamberHit[1]);

		printBothHitCoordinates();
		std::cout << std::endl << "Path Parameters: " << std::endl;
		m_particlePath.printPathParameters();

		std::cout << "Test 63" << std::endl; // Provides a way to quickly check if the program compiled successfully
		
		std::vector <double> vectorX1(0); //defines eight vectors, one for each view in chamber one and again for chamber 2, X,Y,V,U
		std::vector <double> vectorY1(0);	
		std::vector <double> vectorV1(0);
		std::vector <double> vectorU1(0); 
		std::vector <double> vectorX2(0);
		std::vector <double> vectorY2(0);	
		std::vector <double> vectorV2(0);
		std::vector <double> vectorU2(0);  
		std::cout<<getNumberOfSrbs()<<std::endl; //returns number of Srbs=32
		
		std::vector <std::vector <double> * > vectors(0); //vector which stores the eight vectors defined above, assigned below
				
		vectors.push_back(&vectorX1);
		vectors.push_back(&vectorY1); 
		vectors.push_back(&vectorV1);
		vectors.push_back(&vectorU1);
		vectors.push_back(&vectorX2);
		vectors.push_back(&vectorY2);
		vectors.push_back(&vectorV2);
		vectors.push_back(&vectorU2);
				
		std::cout<< "VECTOR SIZE " << vectors.size()<<std::endl; //Ensures the vector is filled
				
		int j;
		for(int i=0;i<16;i++) { 
			j=(floor) (i/2);
			loadSrbAndData(i);
			StrawData strawData(m_hitData);		
			vectors[j]->push_back(strawData.getStrawDistance());	
			std::cout<<"For SRB "<<(i+1)<<" the hit coordinate is... "<< (double) strawData.getStrawDistance()<<std::endl;			
		}  
	} 

	// Private Functions
	void StrawL1Algorithm::loadSrbAndData(int srbNo) {
		loadSrb(srbNo);
		loadData();
	}

	void StrawL1Algorithm::findHitsInChamber(int chamberNo) {
		double hitDisplacementArray[4];
		for (int i = chamberNo*8; i < (chamberNo+1)*8; i+=2) {
			loadSrbAndData(i);
			StrawData strawData(m_hitData);
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