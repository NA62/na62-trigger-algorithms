/*
 * ChamberCluster.h
 *
 *  Created on: 08/ott/2014
 *      Author: jpinzino
 */

#ifndef CHAMBERCLUSTER_H_
#define CHAMBERCLUSTER_H_
namespace na62
    {
    class ViewCluster;
    }

namespace na62
    {

    class ChamberCluster
	{
    public:

	int npoints;
	float* z;
	float* x;
	float* y;
	float* trailing;
	int* used;

	ChamberCluster();
	virtual ~ChamberCluster();

	void SetCluster(ViewCluster *, float);
	};

    } /* namespace na62 */

#endif /* CHAMBERCLUSTER_H_ */
