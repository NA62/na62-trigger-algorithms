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
	static double rTDependence(double);
	static double rTParametricDependence(double);
	static double rTDependenceData(double);

private:
};

} /* namespace na62 */

#endif /* L1_STRAW_ALGORITHM_DIGIMANAGER_H_ */
