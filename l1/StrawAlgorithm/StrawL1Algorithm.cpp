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
#include "StrawDetectorParameters.h"

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

		std::cout << "Test 131" << std::endl; // Provides a way to quickly check if the program compiled successfully
							
		StrawViewVectors strawViewVectors;
		
		//THIS WORKS FOR THE FIRST HIT IN THE SRB
		int j;
		for(int i = 0; i < 16; i++) { 
			j=(int) (i/2.0);
			loadSrbAndData(i);
			StrawData strawData(m_hitData);	
			strawViewVectors.pushBackVector(j, strawData.getStrawDistance());
			//std::cout << "For SRB "<< i << " the hit coordinate is:  " << strawData.getStrawDistance() << std::endl;
		}	
			
		/*
		* This code first loops through the 'vectors' vector, for each vector, it then loops
		* through all the elements, if this vector is in chamber 1, it then loops through the
		* chamber 2 vectors to find whether the path falls within the decay length, if it is
		* in chamber 2, it looks in chamber 1. 
		*/ 
		
		double  theta=0.246, error=0.01;
		int temp,number=0;
		for(int i=0;i<strawViewVectors.vectors.size();i++) { 
			for(int j=0; j<strawViewVectors.getVectorSize(i); j++) {
				double length1=strawViewVectors.getVectorValue(i,j)/tan(theta);
				if( i>=0||i<=3) temp=4;
				else {temp=0;}
				for(int k=temp;k<(temp+4);k++) {
					for(int l=0; l<strawViewVectors.getVectorSize(k); l++) {
						double length2=strawViewVectors.getVectorValue(k,l)/tan(theta);
						if ((length1>((1-error)*length2))&&(length1<((1+error)*length2))&&(length1<strawparameters::DECAY_LENGTH)) {
							number++;
							std::cout<<"Found Path"<<std::endl; //Replace this with trigger decision and outputting to file
						} 
					}
				}
			}
		}   
		std::cout<<"The number of path matches is... "<<number<<std::endl;
			
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