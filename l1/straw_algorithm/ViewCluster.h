/*
 * ViewCluster.h
 *
 *  Created on: 30/set/2014
 *      Author: jpinzino
 */

#ifndef VIEWCLUSTER_H_
#define VIEWCLUSTER_H_

namespace na62
    {
    class StrawHits;
    }

namespace na62
    {

    class ViewCluster
	{
    public:

	int ncluster;
	float* coordinate;
	float* trailing;
	int* used;

	ViewCluster();
	virtual ~ViewCluster();
	void SetCluster(StrawHits*, StrawHits*);
	float RadiusReconstructed(int);
	float StrawPos1(int);
	float StrawPos2(int);
	};

    } /* namespace na62 */

#endif /* VIEWCLUSTER_H_ */

//0=v, 1=u, 2=x, 3=y
