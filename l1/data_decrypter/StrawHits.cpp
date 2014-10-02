/*
 * StrawHits.cpp
 *
 *  Created on: Sep 25, 2014
 *      Author: jpinzino
 */

#include "StrawHits.h"
#include "StrawData.h"
#include <stdlib.h>

namespace na62 {

StrawHits::StrawHits() {
	nhit = 0;
	strawID = (int*) calloc (maxnhit,sizeof(int));
	leading = (int*) calloc (maxnhit,sizeof(int));
	trailing = (int*) calloc (maxnhit,sizeof(int));
	used = (int*) calloc (maxnhit,sizeof(int));

}

StrawHits::~StrawHits() {
	// TODO Auto-generated destructor stub
	free(strawID);
	free(leading);
	free(trailing);
	free(used);
}

void StrawHits::SetHits(StrawData data) {

	if (data.nhits > 0) {
		strawID[0] = (int) data.hits[0].strawID;

		if ((int) data.hits[0].edge == 0) //0 leading, 1 trailing
			leading[0] = (int) data.hits[0].fineTime;
		else
			trailing[0] = (int) data.hits[0].fineTime;

		nhit++;
	}

	for (int i = 1; i < (int)data.nhits; i++) {
		int temp = 0;
		for (int j = 0; j < nhit; j++) {
			if ((int) data.hits[i].strawID == strawID[j]) {
				temp = 1;
				if ((int) data.hits[i].edge == 0 && (data.hits[i].fineTime < leading[j] || leading[j] == 0)) //0 leading
					leading[j] = (int) data.hits[i].fineTime;
				else if(data.hits[i].fineTime > trailing[j]) //1 trailing
					trailing[j] = (int) data.hits[i].fineTime;
			}
		}
		if (temp == 0) {
			strawID[nhit] = (int) data.hits[i].strawID;

			if ((int) data.hits[i].edge == 0)
				leading[nhit] = (int) data.hits[i].fineTime;
			else
				trailing[nhit] = (int) data.hits[i].fineTime;

			nhit++;
		}

	}

}

}
