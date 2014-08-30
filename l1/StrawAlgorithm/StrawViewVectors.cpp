/*
 * StrawViewm_vectors.cpp
 *
 *  Created on: Aug 20, 2014
 *      Author: Laura Rogers (laura.kathryn.rogers@cern.ch)
 */

#include "StrawViewVectors.h"

namespace na62 {

StrawViewVectors::StrawViewVectors() {
	initialiseViewVector();
}

void StrawViewVectors::pushBackVector(int i, double input) {
	vectors[i]->push_back(input);
}

void StrawViewVectors::initialiseViewVector() {
	//this vector X = 
	std::vector<double> vectorX1(); //defines eight vectors, one for each view in chamber one and again for chamber 2, X,Y,V,U
	std::vector<double> vectorY1();
	std::vector<double> vectorV1();
	std::vector<double> vectorU1();
	std::vector<double> vectorX2();
	std::vector<double> vectorY2();
	std::vector<double> vectorV2();
	std::vector<double> vectorU2();
	std::vector<std::vector<double> *> vectors; //this vector contains all eight previous vectors, added below

	this->vectors.push_back(&(this->vectorX1));
	this->vectors.push_back(&(this->vectorY1));
	this->vectors.push_back(&(this->vectorV1));
	this->vectors.push_back(&(this->vectorU1));
	this->vectors.push_back(&(this->vectorX2));
	this->vectors.push_back(&(this->vectorY2));
	this->vectors.push_back(&(this->vectorV2));
	this->vectors.push_back(&(this->vectorU2)); //previously all had & for address
	//just changed all vectors to m_vectors 
}

} /* namespace na62 */
