/*
 * TimeCandidate.h
 *
 *  Created on: 24 Apr 2015
 *      Author: vfascian
 */

#ifndef L1_RICH_ALGORITHM_TIMECANDIDATE_H_
#define L1_RICH_ALGORITHM_TIMECANDIDATE_H_

#include <vector>
#include <map>

#define maxNCandHits 500

namespace na62 {
class TimeCandidate {

public:
	TimeCandidate();
	~TimeCandidate();

	void addEdgeIndexes(int edgeIndex, int nTotEdge);
	void removeEdgeIndexes(int nEdgePos);
	void addCandTime(double time);

	int* getEdgeIndexes();
	double getCandTime();
	int getNHits();
	bool getIsSelected();
	bool getIsRemoved();
	double getDeltaTimeClosestCandidate();
	int getNHitsClosestCandidate();
	//std::vector<std::pair<int, double>>* getSortMapX();
	void setDeltaTimeClosestCandidate(double time);
	void setNHitsClosestCandidate(int hits);
	void setNHits(int nTotEdges);
	void setIsSelected(bool selection);
	void setIsRemoved(bool removed);
	//void setSortMapX(std::vector<std::pair<int, double>> map);

private:

	int edgeCandIndexes[maxNCandHits];
	double candTime;
	int nCandHits;
	bool isSelected;
	double deltaTimeClosestCandidate;
	int nHitsClosestCandidate;
	bool isRemoved;

public:

	std::vector<std::pair<int, double>> sortMapX;

};
}
#endif /* L1_RICH_ALGORITHM_TIMECANDIDATE_H_ */
