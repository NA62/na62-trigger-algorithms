/*
 * Track.h
 *
 *  Created on: 15 Mar 2016
 *      Author: romano
 */

#ifndef L1_STRAW_ALGORITHM_TRACK_H_
#define L1_STRAW_ALGORITHM_TRACK_H_

namespace na62 {

class Track {
public:
	Track();
	virtual ~Track();

	int ncentrali; //numero punti nella casella centrale
	int nlaterali; //numero di punti addizionali
	int ncondivisi; //numero punti condivisi con altri traclet
	int ncamcondivise;
	float my;
	float qy;
	float m1x;
	float m2x;
	float q1x;
	float q2x;
	int* hitc; //elenco hit presenti nella casella centrale
	int* camerec; //camere degli hit nella casella centrale
	int* hitl; //elenco hit in altre caselle intorno
	int* camerel; //camere degli hit nelle caselle intorno
//	int hitc[6]; //elenco hit presenti nella casella centrale
//	int camerec[6]; //camere degli hit nella casella centrale
//	int hitl[6]; //elenco hit in altre caselle intorno
//	int camerel[6]; //camere degli hit nelle caselle intorno
	int usato;
	float zvertex;
	float pz;
	float cda;
	double trailing;

	void setTrack( int a,int b,int c,int q,float d, float e, float f, float g, float r, float s, float t, int* h, int *i, int *l, int *m, int o, float p, float u, double z );
	void printTrack();
	void copyTrack(Track track);

	void setHitc(int index, int hit);
	void setChamberc(int index, int chamber);
	void setHitl(int index, int hit);
	void setChamberl(int index, int chamber);

	int getHitc(int index);
	int getChamberc(int index);
	int getHitl(int index);
	int getChamberl(int index);

private:
};

} /* namespace na62 */

#endif /* L1_STRAW_ALGORITHM_TRACK_H_ */
