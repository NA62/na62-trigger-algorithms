/*
 * StrawViewVectors.h
 *
 *  Created on: Aug 20, 2014
 *  Author: Laura Rogers (lr12078@my.bristol.ac.uk)
 *
 * 	Please Note: Laura was a summer student who finished on the 12/09/14. 
 * 	She is no longer working on the code, however can still answer questions.
 */

#pragma once
#ifndef STRAWVIEWVECTOR_H_
#define STRAWVIEWVECTOR_H_

#include <sys/types.h>
#include <cstdint>
#include <iostream>
#include <vector>

#include "StrawData.h"

namespace na62 {

class StrawViewVectors {
public:
	StrawViewVectors();

	inline double getVectorSize(int i) {
		return vectors.at(i)->size();
	}
	inline double getVectorValue(int i, int j) {
		return vectors.at(i)->at(j);
	}

	void pushBackVector(int i, double input);
	std::vector<std::vector<double> *> vectors; //this vector contains all eight previous vectors, added below
	std::vector<double> vectorX1; //defines eight vectors, one for each view in chamber one and again for chamber 2, X,Y,V,U
	std::vector<double> vectorY1;
	std::vector<double> vectorV1;
	std::vector<double> vectorU1;
	std::vector<double> vectorX2;
	std::vector<double> vectorY2;
	std::vector<double> vectorV2;
	std::vector<double> vectorU2;

private:
	void initialiseViewVector();
	// Member Variables

};

} /* namespace na62 */

#endif /* STRAWVIEWVECTOR_H_ */
