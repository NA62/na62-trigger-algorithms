/*
 * Cluster.cpp
 *
 *  Created on: 3 Mar 2016
 *      Author: romano
 */

#include "Cluster.h"

#include <options/Logging.h>

namespace na62 {

Cluster::Cluster() {
	chamber = view = used = 0;
	coordinate = trailing = deltadistance = 0.;
}

Cluster::~Cluster() {
	// TODO Auto-generated destructor stub
}

void Cluster::setCluster(int a, int b, float c, float e, double f, int d) {
	chamber = a;
	view = b;
	coordinate = c;
	used = d;
	trailing = e;
	deltadistance = f;
}

void Cluster::printCluster() {
	printf("%d %d %f %lf %f %d\n", chamber, view, coordinate, trailing,
			deltadistance, used);
}

void Cluster::printCluster2() {
	printf("%d %d %f %lf %f\n", chamber, view, coordinate, trailing,
			deltadistance);
}

} /* namespace na62 */
