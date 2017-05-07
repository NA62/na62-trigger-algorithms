/*
 * Track.cpp
 *
 *  Created on: 15 Mar 2016
 *      Author: romano
 */

#include "Track.h"

#include <options/Logging.h>

namespace na62 {

Track::Track(){
// TODO Auto-generated constructor stub
	ncentrali = nlaterali = ncondivisi = ncamcondivise = usato = 0;
	my = qy = m1x = q1x = m2x = q2x = pz = zvertex = trailing = cda = 0.;
	for ( int i=0; i<6; i++ )
	    hitc[i]=camerec[i]=hitl[i]=camerel[i]=0;
}

Track::~Track() {
	// TODO Auto-generated destructor stub
}

void Track::setTrack(int a, int b, int c, int q, float d, float e, float f,
		float g, float r, float s, float t, int* h, int* i, int* l, int* m,
		int o, float p, float u, double z) {
	ncentrali = a;
	nlaterali = b;
	ncondivisi = c;
	ncamcondivise = q;
	my = d;
	qy = e;
	m1x = f;
	q1x = g;
	m2x = r;
	q2x = s;
	pz = t;
	for (int index = 0; index < 6; index++) {
		hitc[index] = h[index];
		camerec[index] = i[index];
		hitl[index] = l[index];
		camerel[index] = m[index];
	}
	usato = o;
	zvertex = p;
	cda = u;
	trailing = z;
}

void Track::printTrack() {
	printf(
			"nce=%d nl=%d nco=%d ncamco=%d \nm1x=%f q1x=%f m2x=%f q2x=%f my=%f qy=%f \nPz=%f, zvertex=%f cda=%f trailing=%lf, usato=%d\n",
			ncentrali, nlaterali, ncondivisi, ncamcondivise, m1x, q1x, m2x, q2x,
			my, qy, pz, zvertex, cda, trailing, usato);
	printf("centrali\n");
	for (int a = 0; a < ncentrali; a++)
		printf("hit=%d camera=%d\n", hitc[a], camerec[a]);
	printf("laterali\n");
	for (int a = 0; a < nlaterali; a++)
		printf("hit=%d camera=%d\n", hitl[a], camerel[a]);
}

void Track::copyTrack(Track track) {
	ncentrali = track.ncentrali;
	nlaterali = track.nlaterali;
	ncondivisi = track.ncondivisi;
	ncamcondivise = track.ncamcondivise;
	my = track.my;
	qy = track.qy;
	m1x = track.m1x;
	m2x = track.m2x;
	q1x = track.q1x;
	q2x = track.q2x;
	usato = track.usato;
	zvertex = track.zvertex;
	pz = track.pz;
	cda = track.cda;
	trailing = track.trailing;

	for (int i = 0; i < track.ncentrali; i++) {
		hitc[i] = track.hitc[i];
		camerec[i] = track.camerec[i];
	}
	for (int i = 0; i < track.nlaterali; i++) {
		hitl[i] = track.hitl[i];
		camerel[i] = track.camerel[i];
	}
}

void Track::setHitc(int index, int hit) {
	hitc[index] = hit;
}
void Track::setChamberc(int index, int chamber) {
	camerec[index] = chamber;
}
void Track::setHitl(int index, int hit) {
	hitl[index] = hit;
}
void Track::setChamberl(int index, int chamber) {
	camerel[index] = chamber;
}

int Track::getHitc(int index) {
	return hitc[index];
}
int Track::getChamberc(int index) {
	return camerec[index];
}
int Track::getHitl(int index) {
	return hitl[index];
}
int Track::getChamberl(int index) {
	return camerel[index];
}
} /* namespace na62 */
