/*
 * TimeCandidate.cpp\
 *
 *  Created on: 24 Apr 2015
 *      Author: vfascian
 */

#include <options/Logging.h>
#include <vector>
#include <map>

#include "TimeCandidate.h"

namespace na62 {

TimeCandidate::TimeCandidate() {

	candTime = 0;
	nCandHits = 0;
	deltaTimeClosestCandidate = 0;
	nHitsClosestCandidate = -1;
	isSelected = false;
	isRemoved = false;
	//sortMapX = nullptr;

	for (int i = 0; i < maxNCandHits; ++i) {
		edgeCandIndexes[i] = -1;
	}
}

TimeCandidate::~TimeCandidate() {
}

void TimeCandidate::addEdgeIndexes(int edgeIndex, int nTotEdge) {
	edgeCandIndexes[nTotEdge] = edgeIndex; // Pay attention to the incrementation of nTotEdge in TimeClustering!!!!
}

void TimeCandidate::removeEdgeIndexes(int nEdgePos) {
	edgeCandIndexes[nEdgePos] = -1;
}

void TimeCandidate::addCandTime(double time) {
	candTime = time;
}

int* TimeCandidate::getEdgeIndexes() {
	return edgeCandIndexes;
}

double TimeCandidate::getCandTime() {
	return candTime;
}

void TimeCandidate::setNHits(int nTotEdges) {
	nCandHits = nTotEdges;
}

int TimeCandidate::getNHits() {
	return nCandHits;
}

void TimeCandidate::setIsSelected(bool selection) {
	isSelected = selection;
}

bool TimeCandidate::getIsSelected() {
	return isSelected;
}

void TimeCandidate::setDeltaTimeClosestCandidate(double time) {
	deltaTimeClosestCandidate = time;
}

void TimeCandidate::setNHitsClosestCandidate(int hits) {
	nHitsClosestCandidate = hits;
}

double TimeCandidate::getDeltaTimeClosestCandidate() {
	return deltaTimeClosestCandidate;
}

int TimeCandidate::getNHitsClosestCandidate() {
	return nHitsClosestCandidate;
}

void TimeCandidate::setIsRemoved(bool removed) {
	isRemoved = removed;
}

bool TimeCandidate::getIsRemoved() {
	return isRemoved;
}

//std::vector<std::pair<int, double>>* TimeCandidate::getSortMapX() {
//	return sortMapX;
//}

//void TimeCandidate::setSortMapX(std::vector<std::pair<int, double>> map){
//	sortMapX = map;
//}

}
