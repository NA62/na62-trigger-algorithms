/*
 * StrawL1Algorithm.cpp
 *
 *  Created on: Aug 7, 2014
 *	Author: Thomas Hancock (Thomas.hancock.2012@my.bristol.ac.uk)
 *	Modified: Laura Rogers (lr12078@my.bristol.ac.uk)
 *
 *	Please Note: Thomas and Laura were both summer students, who finished on the 
 *	12/09/14. They are no longer working on the code, however, can still answer questions.
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

		std::cout << "Test 175" << std::endl; // Provides a way to quickly check if the program compiled successfully
							
		StrawViewVectors strawViewVectors;
		
		int j;
		for(int i = 0; i < 16; i++) { 
			j=(int) (i/2.0);
			loadSrbAndData(i);
			StrawData strawData(m_hitData);	
			strawViewVectors.pushBackVector(j, strawData.getStrawDistance());
		}
		
		/* 
		 * The code below loops through the x and y views in chamber 1 and 2, and loops 
		 * through all the hits. If a hit is 0, it calculates what the hit should be from the other 
		 * coordinates.
		 */
		
		for(int i=0;i<8;i++) {
			if(i==0||i==1) {
				for(int j=0; j<strawViewVectors.getVectorSize(i); j++) {
					if(strawViewVectors.getVectorValue(i,j)==0) {
						strawViewVectors.vectors.at(i)->at(j)=findMissingCoordinate(strawViewVectors.getVectorValue(0,j), strawViewVectors.getVectorValue(1,j), strawViewVectors.getVectorValue(2,j), strawViewVectors.getVectorValue(3,j)); 
					}
				}
			}
			if(i==4||i==5) {
				for(int k=0; k<strawViewVectors.getVectorSize(i); k++) {
					if(strawViewVectors.getVectorValue(i,k)==0) {
						strawViewVectors.vectors.at(i)->at(k)=findMissingCoordinate(strawViewVectors.getVectorValue(4,k), strawViewVectors.getVectorValue(5,k), strawViewVectors.getVectorValue(6,k), strawViewVectors.getVectorValue(7,k)); 
					}
				}
			}
		}
		
		/*
		 * The code below loops through all the hits in chamber 1, and works out the path
		 * for all combinations of hits in chamber 2. If the paths intersect the beam 
	 	 * line, within the decay volume, a path has been found.
		 */
		
		
		double xLength, yLength, closestDistance, error=34; //error in mm
		int number=0,number2=0;
		for(int j=0; j<strawViewVectors.getVectorSize(0); j++) {
			//std::cout<<"View x, ch 1 "<< "   Hit "<<j<<"   "<<strawViewVectors.vectors.at(0)->at(j)<<std::endl;
			for(int k=0; k<strawViewVectors.getVectorSize(4); k++) {
				//std::cout<<"View x, ch 2 "<< "   Hit "<<k<<"   "<<strawViewVectors.vectors.at(4)->at(k)<<std::endl;
				xLength=findLength(strawViewVectors.vectors.at(0)->at(j),strawViewVectors.vectors.at(4)->at(k));
				for(int l=0; l<strawViewVectors.getVectorSize(1); l++) {	
					//std::cout<<"View y, ch 1 "<< "   Hit "<<l<<"   "<<strawViewVectors.vectors.at(1)->at(l)<<std::endl;
					for(int m=0; m<strawViewVectors.getVectorSize(5); m++) {	
						//std::cout<<"View y, ch 2 "<< "   Hit "<<m<<"   "<<strawViewVectors.vectors.at(5)->at(m)<<std::endl;
						yLength=findLength(strawViewVectors.vectors.at(1)->at(l),strawViewVectors.vectors.at(5)->at(m));
						closestDistance=fabs(yLength-xLength);
						std::cout<<"xLength is "<<xLength<<std::endl;
						std::cout<<"yLength is "<<yLength<<std::endl;
						std::cout<<"Closest Distance is "<<closestDistance<<" mm"<<'\n'<<std::endl;
						if (xLength<strawparameters::DECAY_LENGTH&&yLength<(strawparameters::DECAY_LENGTH+distanceFromXView(1))&&(xLength>(yLength-error))&&(xLength<(yLength+error))) {
							number++;
							} else {number2++;}
					}
				}
			}
		}
		std::cout<<"The number of path matches is... "<<number<<std::endl;
		std::cout<<"The number of unmatched paths is... "<<number2<<std::endl;	
	} 

	// Private Functions
	void StrawL1Algorithm::loadSrbAndData(int srbNo) {
		loadSrb(srbNo);
		loadData();
	}
	
	//Extracts the first hit in each plane of each view to test the reconstruction
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
	
	//prints the binaries out, used for debugging
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
		std::cout << std::endl << "Hit chamber 1:" << std::endl;
		m_chamberHit[0].printCoordinates();	
		std::cout << std::endl << "Hit chamber 2:" << std::endl;
		m_chamberHit[1].printCoordinates();
	}
	
	//Returns the distance of the Straw from the x view (straws between 0 and 7)
	double StrawL1Algorithm::distanceFromXView(int i) {
		if(i==0||i==4) {
			return 0;
		} else if(i==2||i==5) { 
			return strawparameters::UV_AND_XY_SPACING;
		} else if(i==3||i==6) {
			return (strawparameters::UV_AND_XY_SPACING)+(strawparameters::VX_SPACING);
		} else {
			return 2*(strawparameters::UV_AND_XY_SPACING)+(strawparameters::VX_SPACING);
		}
	}
	
	//Returns the length along the beam line 'plane' until it intercepts it
	double StrawL1Algorithm::findLength(double i, double j) {
		return ((i*strawparameters::CHAMBERS_ONE_AND_TWO_SPACING*1000)/(j-i));
	} 
	
	//If a coordinate is missing, it is calculated and returned using the other views hit coordinates. This function is from the HitCoordinate class 
	double StrawL1Algorithm::findMissingCoordinate(double x, double y, double v, double u) {
		if ((x == 0)&&(v != 0)&&(y!=0)) {
			return M_SQRT2*v - y;
		} else if ((x == 0)&&(u != 0)&&(y!=0)) {
			return M_SQRT2*u + y;
		} else if ((y == 0)&&(v != 0)&&(x!=0)) {
			return M_SQRT2*v - x;
		} else if ((y == 0)&&(u != 0)&&(x!=0)) {
			return x - M_SQRT2*u;
		} else if ((x==0)&&(v != 0)&&(u!=0)) {
			return (v-u)/(2*M_SQRT2);
		} else if ((y==0)&&(v != 0)&&(u!=0)) {
			return (v+u)/(2*M_SQRT2);
		} else { 
			return 0; //This is a debug, it should never return 0
		}
	}
	
} /* namespace na62 */