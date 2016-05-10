/*
 * DigiManager.h
 *
 *  Created on: 25 Feb 2016
 *      Author: romano
 */

#ifndef L1_STRAW_ALGORITHM_DIGIMANAGER_H_
#define L1_STRAW_ALGORITHM_DIGIMANAGER_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

namespace na62 {

class DigiManager {
public:
	DigiManager();
	virtual ~DigiManager();
	double rTDependence(double);
	double rTParametricDependence(double);
	double rTDependenceData(double);

private:
};

} /* namespace na62 */

#endif /* L1_STRAW_ALGORITHM_DIGIMANAGER_H_ */
