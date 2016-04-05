/*
 * Straw.h
 *
 *  Created on: 25 Feb 2016
 *      Author: romano
 */

#ifndef L1_STRAW_ALGORITHM_STRAW_H_
#define L1_STRAW_ALGORITHM_STRAW_H_

namespace na62 {

class Straw {

public:
	Straw();
	virtual ~Straw();

	int chamber;
	int view;
	int halfview;
	int plane;
	int tube;
	double leading;
	double trailing;
	int used;
	int srbid;
	float position;
	float wiredistance;

	void setStraw(int a,int b,int c,int d, int e,double f, double g, int h, int i, float l, float m);
	void printStraw();

private:
};

} /* namespace na62 */

#endif /* L1_STRAW_ALGORITHM_STRAW_H_ */
