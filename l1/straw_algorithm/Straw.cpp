/*
 * Straw.cpp
 *
 *  Created on: 25 Feb 2016
 *      Author: romano
 */

#include "Straw.h"

#include <options/Logging.h>

namespace na62 {

Straw::Straw() {
	chamber = view = halfview = plane = tube = used = srbid = 0;
	leading = trailing = -1000.0;
	position = -10000.0;
	wiredistance = -100.0;
}

Straw::~Straw() {
}

void Straw::printStraw() {
	printf("(%d,%d,%d,%d,%d,%lf,%lf,%d,%f,%f)\n", chamber, view, halfview,
			plane, tube, leading, trailing, used, position, wiredistance);
}

void Straw::setStraw(int a, int b, int c, int d, int e, double f, double g,
		int h, int i, float l, float m) {
	chamber = a;
	view = b;
	halfview = c;
	plane = d;
	tube = e;
	leading = f;
	trailing = g;
	used = h;
	srbid = i;
	position = l;
	wiredistance = m;
}

} /* namespace na62 */
