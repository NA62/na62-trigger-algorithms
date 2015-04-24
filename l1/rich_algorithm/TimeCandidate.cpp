/*
 * TimeCandidate.cpp\
 *
 *  Created on: 24 Apr 2015
 *      Author: vfascian
 */

#include "TimeCandidate.h"

TimeCandidate::TimeCandidate() {
	candTime = 0;
}

TimeCandidate::~TimeCandidate() {
}

void TimeCandidate::AddEdgeIndexes(int edgeIndex, int nTotEdge) {
	edgeCandIndexes[nTotEdge] = edgeIndex; // Pay attention to the incrementation of nTotEdge in TimeClustering!!!!
}

//void AddCandTime(double time){
//	candTime = time;
//}

double* TimeCandidate::GetEdgeIndexes() {

	return &edgeCandIndexes[0];
}

double TimeCandidate::GetCandTime(){

	return candTime;
}
