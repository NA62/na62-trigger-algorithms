/*
 * DigiManager.cpp
 *
 *  Created on: 25 Feb 2016
 *      Author: romano
 */

#include "DigiManager.h"

namespace na62 {

// RT dependence for full digitization
double DigiManager::rTDependence(double time) {
	double time2 = time * time;
	if (time < 0)
		return -(-0.072766 + 0.1665678 * time - 0.005184744 * time2
				+ 9.83302e-05 * time2 * time - 6.97656e-07 * time2 * time2);
	else if (time < 20)
		return (-0.072766 + 0.1665678 * time - 0.005184744 * time2
				+ 9.83302e-05 * time2 * time - 6.97656e-07 * time2 * time2);
	else
		return (0.861513 + 0.0559724 * time - 0.000197694 * time2);
}

// RT for parametric digitization (in mus)
double DigiManager::rTParametricDependence(double tt) {
	double pol[10];
	pol[0] = 1.40694;
	pol[1] = -288.673;
	pol[2] = 22372.4;
	pol[3] = -724749;
	pol[4] = 1.38739e+07;
	pol[5] = -1.69746e+08;
	pol[6] = 1.34466e+09;
	pol[7] = -6.68268e+09;
	pol[8] = 1.89464e+10;
	pol[9] = -2.33756e+10;

	double f1 = 0;
	double d = 1;

	if (tt < 0.02) {
		double val = 0;
		for (int j = 0; j != 10; ++j) {
			val += pol[j] * d;
			d *= 0.02;
		}
		double b = val / (0.02 - 0.0155);
		double a = -0.0155 * b;
		f1 = a + b * tt;
		if (f1 < 0)
			f1 = 0;
	}

	if (tt >= 0.02 && tt <= 0.09) {
		for (int j = 0; j != 10; ++j) {
			f1 += pol[j] * d;
			d *= tt;
		}
	}

	if (tt > 0.09 && tt <= 0.115) {
		double val = 0;
		for (int j = 0; j < 10; j++) {
			val += pol[j] * d;
			d *= 0.09;
		}
		double b = (4.26 - val) / (0.115 - 0.09);
		double a = 4.26 - 0.115 * b;
		f1 = a + b * tt;
	}

	if (tt > 0.115 && tt <= 0.15) {
		double b = (4.82767 - 4.26) / (0.15 - 0.115);
		double a = 4.82767 - 0.15 * b;
		f1 = a + b * tt;
	}

	if (tt > 0.15) {
		f1 = 4.82767;
	}

	double r = f1 - 0.017;

	return r > 0 ? r : 0;
}

double DigiManager::rTDependenceData(double tt) {

	double par[5] = { -2.29722e+00, 7.63831e+02, 8.07073e-03, 2.25652e+01,
			-5.89754e+01 };

	double r = par[0]
			+ sqrt(
					par[1] * (tt - par[2])
							+ par[4] * (tt - par[2]) * (tt - par[2]))
			- par[3] * tt;
//  if (par[1]*(tt-par[2])+par[4]*(tt-par[2])*(tt-par[2])<0) r = 0;
	if (par[1] * (tt - par[2]) + par[4] * (tt - par[2]) * (tt - par[2]) < 0)
		r = par[0]
				+ sqrt(
						-(par[1] * (tt - par[2])
								+ par[4] * (tt - par[2]) * (tt - par[2])))
				- par[3] * tt;
//  return r>=0?r:-r;
	if (r < 0)
		r = 0;
	return r;

}

} /* namespace na62 */
