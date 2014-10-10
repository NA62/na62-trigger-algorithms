/*
 * cut.h
 *
 *  Created on: 01/ott/2014
 *      Author: jpinzino
 */

#ifndef CUT_H_
#define CUT_H_

//pre-cut
#define lowLeading 0 //ns <- to set because leading and trailng isn't in ns
#define highLeading 140
#define lowHitTrailing 0
#define highHitTrailing 270

//view clustering
#define lowDeltaDistance -2.4  //mm
#define highDeltaDistance 2.6

//chamber clustering
#define clusterDistance 6.0 //mm
#define lowClusterTrailing 100.0 //ns
#define highClusterTrailing 200.0

//pattern recognition
#define lowTrackTrailing 120.0 //ns
#define highTrackTrailing 180.0
#define lowDqx -30.0 //mm
#define highDqx 20.0

//physics cut
#define lowVertex 90000 //mm
#define highVertex 180000
#define lowPz 10000 //MeV
#define HighPz 45000


//cte

#define maxNhit 200
#define strawSeparator 60


#endif /* CUT_H_ */
