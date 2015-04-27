/*
 * TimeCandidate.h
 *
 *  Created on: 24 Apr 2015
 *      Author: vfascian
 */

#ifndef L1_RICH_ALGORITHM_TIMECANDIDATE_H_
#define L1_RICH_ALGORITHM_TIMECANDIDATE_H_

#define maxNCandHits 50

namespace na62{
class TimeCandidate {

public:
	TimeCandidate();
	~TimeCandidate();


	void addEdgeIndexes(int edgeIndex, int nTotEdge);
	void addCandTime(double time);

	int* getEdgeIndexes();
	double getCandTime();
	int getNHits();

private:

	int edgeCandIndexes[maxNCandHits];
	double candTime;
	int nHits;

};
}
#endif /* L1_RICH_ALGORITHM_TIMECANDIDATE_H_ */
