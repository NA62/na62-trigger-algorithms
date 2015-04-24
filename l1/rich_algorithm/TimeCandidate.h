/*
 * TimeCandidate.h
 *
 *  Created on: 24 Apr 2015
 *      Author: vfascian
 */

#ifndef L1_RICH_ALGORITHM_TIMECANDIDATE_H_
#define L1_RICH_ALGORITHM_TIMECANDIDATE_H_

#define maxNHits 250


class TimeCandidate {

public:
	TimeCandidate();
	~TimeCandidate();


	void AddEdgeIndexes(int edgeIndex, int nTotEdge);
	void AddCandTime(double candTime);
	double* GetEdgeIndexes();
	double GetCandTime();

private:

	double edgeCandIndexes[maxNHits];
	double candTime;

};

#endif /* L1_RICH_ALGORITHM_TIMECANDIDATE_H_ */
