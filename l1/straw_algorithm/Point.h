/*
 * Point.h
 *
 *  Created on: 7 Mar 2016
 *      Author: romano
 */

#ifndef L1_STRAW_ALGORITHM_POINT_H_
#define L1_STRAW_ALGORITHM_POINT_H_

namespace na62 {

class Point {
public:
	Point();
	Point(float a, float b, float c, double f, float e, int g, int d);
	virtual ~Point();

	float x;
	float y;
	float z;
	double trailing;
	float viewDistance;
	int nViews;
	int used;

	void setPoint(float a,float b,float c,double f,float e,int g,int d);
	void printPoint();
	void printPoint2 ();
	void clonePoint(Point p);

private:
};

} /* namespace na62 */

#endif /* L1_STRAW_ALGORITHM_POINT_H_ */
