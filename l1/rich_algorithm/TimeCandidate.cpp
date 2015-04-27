/*
 * TimeCandidate.cpp\
 *
 *  Created on: 24 Apr 2015
 *      Author: vfascian
 */

#include "TimeCandidate.h"

namespace na62 {

TimeCandidate::TimeCandidate() {

	candTime = 0;
	nHits = 0;

	for (int i = 0; i < maxNCandHits; ++i) {
		edgeCandIndexes[i] = -1;
	}

}

TimeCandidate::~TimeCandidate() {
}

void TimeCandidate::addEdgeIndexes(int edgeIndex, int nTotEdge) {
	edgeCandIndexes[nTotEdge] = edgeIndex; // Pay attention to the incrementation of nTotEdge in TimeClustering!!!!
}

void TimeCandidate::addCandTime(double time){
	candTime = time;
}

int* TimeCandidate::getEdgeIndexes() {

	return &edgeCandIndexes[0];
}

double TimeCandidate::getCandTime() {

	return candTime;
}

int TimeCandidate::getNHits() {

	for (int i = 0; i < maxNCandHits; ++i) {

		if (edgeCandIndexes[i] > -1)
			nHits++;
	}

	return nHits;
}
}
