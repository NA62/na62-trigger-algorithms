/*
 * Cluster.h
 *
 *  Created on: 3 Mar 2016
 *      Author: romano
 */

#ifndef L1_STRAW_ALGORITHM_CLUSTER_H_
#define L1_STRAW_ALGORITHM_CLUSTER_H_

namespace na62 {

class Cluster {
public:
	Cluster();
	virtual ~Cluster();

	int chamber;
	int view;
	float coordinate;
	double trailing;
	float deltadistance;
	int used;

	void setCluster(int a, int b, float c, float e, double f, int d);
	void printCluster();
	void printCluster2();

private:
};

} /* namespace na62 */

#endif /* L1_STRAW_ALGORITHM_CLUSTER_H_ */
