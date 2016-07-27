/*
 * Point.cpp
 *
 *  Created on: 7 Mar 2016
 *      Author: romano
 */

#include "Point.h"

#include <options/Logging.h>

namespace na62 {

Point::Point() {
	used = nViews = 0;
	x = y = z = trailing = viewDistance = 0.;
}

Point::Point(float a, float b, float c, double f, float e, int g, int d)
{
	setPoint(a, b, c, f, e, g, d);
}

Point::~Point() {
	// TODO Auto-generated destructor stub
}

void Point::setPoint(float a, float b, float c, double f, float e, int g, int d) {
	z = a;
	x = b;
	y = c;
	trailing = f;
	viewDistance = e;
	nViews = g;
	used = d;
}

void Point::printPoint() {
	printf("%f %f %f %lf %f %d %d\n", z, x, y, trailing, viewDistance, nViews,
			used);
}

void Point::printPoint2() {
	printf("%f %f %f %lf %f %d\n", z, x, y, trailing, viewDistance, nViews);
}

void Point::clonePoint(Point p) {
	z = p.z;
	x = p.x;
	y = p.y;
	trailing = p.trailing;
	viewDistance = p.viewDistance;
	used = p.used;
	nViews = p.nViews;
}
} /* namespace na62 */
