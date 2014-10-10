/*
 * ViewCluster.cpp
 *
 *  Created on: 30/set/2014
 *      Author: jpinzino
 */

#include "ViewCluster.h"

#include <stdlib.h>

#include "../data_decrypter/StrawHits.h"
#include "cut.h"

namespace na62
    {

    ViewCluster::ViewCluster()
	{
	// TODO Auto-generated constructor stub
	ncluster = 0;
	coordinate = (float*) calloc(maxNhit, sizeof(float));
	trailing = (float*) calloc(maxNhit, sizeof(float));
	used = (int*) calloc(maxNhit, sizeof(int));

	}

    ViewCluster::~ViewCluster()
	{
	// TODO Auto-generated destructor stub
	}

    float ViewCluster::RadiusReconstructed(int leading)
	{
	return leading;
	}

    float ViewCluster::StrawPos1(int strawId)
	{
	float dist = 17.6;

	float spostamento[2] =
	    {
	    +dist / 8, -dist * 3 / 8
	    };
	float posizione = 0;

	if (strawId < 1000)
	    posizione = (strawId - 56) * dist + spostamento[0];
	else
	    posizione = (strawId - 56) * dist + spostamento[1];

	return posizione;
	}
    float ViewCluster::StrawPos2(int strawId)
	{
	float dist = 17.6;

	float spostamento[2] =
	    {
	    +dist * 3 / 8, -dist / 8
	    };
	float posizione = 0;

	if (strawId < 1000)
	    posizione = (strawId - 56) * dist + spostamento[0];
	else
	    posizione = (strawId - 56) * dist + spostamento[1];

	return posizione;
	}

    void ViewCluster::SetCluster(StrawHits* firstPlane, StrawHits* secondPlane)
	{

	ncluster = 0;

	float position1 = 0.0;
	float position2 = 0.0;
	float meanDistance = 0.0;
	float deltaDistance = 0.0;

	for (int j = 0; j < secondPlane->nhit; j++) //hit loop
	    {
	    for (int h = 0; h < firstPlane->nhit && secondPlane->used[j] == 0; h++) //I take an hit belonged to plane 2 o 3
		{

		float wireDistance1 = RadiusReconstructed(firstPlane->leading[h] / 1000);
		float wireDistance2 = RadiusReconstructed(secondPlane->leading[j] / 1000);

		if (secondPlane->strawID[j] < 1000) //To know the real condiction: plane 0 of second half plane
		    {
		    if (firstPlane->strawID[h] < 1000 && firstPlane->strawID[h] == secondPlane->strawID[j] && firstPlane->used[h] == 0)
			{

			position1 = StrawPos1(firstPlane->strawID[h]) + wireDistance1;
			position2 = StrawPos2(secondPlane->strawID[j]) - wireDistance2;
			meanDistance = (position2 + position1) / 2.0;

			deltaDistance = wireDistance1 + wireDistance2 - 4.4;

			if (deltaDistance < highDeltaDistance && deltaDistance > lowDeltaDistance)
			    {
			    coordinate[ncluster] = meanDistance;
			    trailing[ncluster] = (firstPlane->trailing[h] + secondPlane->trailing[j]) / 2.0;

			    ncluster++;
			    secondPlane->used[j] = 1;
			    firstPlane->used[h] = 1;
			    }
			}
		    else if (firstPlane->strawID[h] > 1000 && firstPlane->strawID[h] == (secondPlane->strawID[j] + 1) && firstPlane->used[h] == 0)
			{

			position1 = StrawPos1(firstPlane->strawID[h]) - wireDistance1;
			position2 = StrawPos2(secondPlane->strawID[j]) + wireDistance2;
			meanDistance = (position1 + position2) / 2.0;

			deltaDistance = wireDistance1 + wireDistance2 - 4.4;

			if (deltaDistance < highDeltaDistance && deltaDistance > lowDeltaDistance)
			    {
			    coordinate[ncluster] = meanDistance;
			    trailing[ncluster] = (firstPlane->trailing[h] + secondPlane->trailing[j]) / 2.0;

			    ncluster++;
			    secondPlane->used[j] = 1;
			    firstPlane->used[h] = 1;
			    }

			}

		    }
		else
		    {
		    if (firstPlane->strawID[h] < 1000 && firstPlane->strawID[h] == secondPlane->strawID[j] && firstPlane->used[h] == 0)
			{

			position1 = StrawPos1(firstPlane->strawID[h]) - wireDistance1;
			position2 = StrawPos2(secondPlane->strawID[j]) + wireDistance2;
			meanDistance = (position1 + position2) / 2;

			deltaDistance = wireDistance1 + wireDistance2 - 4.4;

			if (deltaDistance < highDeltaDistance && deltaDistance > lowDeltaDistance)
			    {
			    coordinate[ncluster] = meanDistance;
			    trailing[ncluster] = (firstPlane->trailing[h] + secondPlane->trailing[j]) / 2.0;

			    ncluster++;
			    secondPlane->used[j] = 1;
			    firstPlane->used[h] = 1;
			    }
			}
		    else if (firstPlane->strawID[h] > 1000 && firstPlane->strawID[h] == secondPlane->strawID[j] && firstPlane->used[h] == 0)
			{

			position1 = StrawPos1(firstPlane->strawID[h]) + wireDistance1;
			position2 = StrawPos2(secondPlane->strawID[j]) - wireDistance2;
			meanDistance = (position1 + position2) / 2;

			deltaDistance = wireDistance1 + wireDistance2 - 4.4;

			if (deltaDistance < highDeltaDistance && deltaDistance > lowDeltaDistance)
			    {
			    coordinate[ncluster] = meanDistance;
			    trailing[ncluster] = (firstPlane->trailing[h] + secondPlane->trailing[j]) / 2.0;

			    ncluster++;
			    secondPlane->used[j] = 1;
			    firstPlane->used[h] = 1;
			    }
			}
		    }

		}
	    }

	}
    }/* namespace na62 */
