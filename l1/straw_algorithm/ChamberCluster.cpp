/*
 * ChamberCluster.cpp
 *
 *  Created on: 08/ott/2014
 *      Author: jpinzino
 */

#include "ChamberCluster.h"

#include <stdlib.h>
#include <cmath>

#include "cut.h"
#include "ViewCluster.h"

namespace na62
    {

    ChamberCluster::ChamberCluster()
	{
	// TODO Auto-generated constructor stub
	npoints = 0;
	z = (float*) calloc(maxNhit, sizeof(float));
	x = (float*) calloc(maxNhit, sizeof(float));
	y = (float*) calloc(maxNhit, sizeof(float));
	trailing = (float*) calloc(maxNhit, sizeof(float));
	used = (int*) calloc(maxNhit, sizeof(int));

	}

    ChamberCluster::~ChamberCluster()
	{
	// TODO Auto-generated destructor stub
	free(z);
	free(x);
	free(y);
	free(trailing);
	free(used);
	}

    void ChamberCluster::SetCluster(ViewCluster* view, float chamberCoordiante) //0=v, 1=u, 2=x, 3=y
	{

	float coordinate_temp;
	float distance;

	for (int a = 0; a < view[3].ncluster; a++)
	    {

	    coordinate_temp = 0.0;
	    distance = 0.0;

	    for (int b = 0; b < view[2].ncluster; b++) //if clusters[a] is inside y view we loop on x view
		{

		for (int c = 0; c < view[0].ncluster; c++) //if cluster [b] is inside x view we loop on u and v views
		    {
		    coordinate_temp = (view[3].coordinate[a] + view[2].coordinate[b]) / sqrt(2);
		    distance = fabs(view[0].coordinate[c] - coordinate_temp);

		    if (distance < clusterDistance)
			{
			if ((view[3].used[a] == 0 || view[2].used[b] == 0 || view[0].used[c] == 0) && (view[2].coordinate[b] != x[npoints - 1] || view[3].coordinate[a] != y[npoints - 1]))
			    {
			    z[npoints] = chamberCoordiante;
			    x[npoints] = view[2].coordinate[b];
			    y[npoints] = view[3].coordinate[a];
			    trailing[npoints] = (view[0].trailing[c] + view[2].trailing[b] + view[3].trailing[a]) / 3;

			    npoints++;

			    view[3].used[a] = 1;
			    view[2].used[b] = 1;
			    view[0].used[c] = 1;

			    }
			else
			    {
			    if (view[3].used[a] == 2)
				view[0].used[c] = 1;

			    view[3].used[a] = 1;
			    view[2].used[b] = 1;
			    }

			}
		    }
		for (int c = 0; c < view[1].ncluster; c++) //if cluster [b] is inside x view we loop on u and v views
		    {
		    coordinate_temp = (view[2].coordinate[b] - view[3].coordinate[a]) / sqrt(2);
		    distance = fabs(view[1].coordinate[c] - coordinate_temp);

		    if (distance < clusterDistance)
			{

			if ((view[3].used[a] == 0 || view[2].used[b] == 0 || view[1].used[c] == 0) && (view[2].coordinate[b] != x[npoints - 1] || view[3].coordinate[a] != y[npoints - 1]))
			    {

			    z[npoints] = chamberCoordiante;
			    x[npoints] = view[2].coordinate[b];
			    y[npoints] = view[3].coordinate[a];
			    trailing[npoints] = (view[1].trailing[c] + view[2].trailing[b] + view[3].trailing[a]) / 3;

			    npoints++;

			    view[3].used[a] = 2;
			    view[2].used[b] = 1;
			    view[1].used[c] = 1;
			    }
			else
			    {
			    if (view[3].used[a] == 1)
				view[1].used[c] = 1;

			    view[3].used[a] = 1;
			    view[2].used[b] = 1;
			    }

			}

		    }

		}

	    }

	//2° cluster loop: (v,u) looking for x or y //0=v, 1=u, 2=x, 3=y

	for (int a = 0; a < view[0].ncluster; a++)
	    {

	    coordinate_temp = 0.;
	    distance = 0.;

	    for (int b = 0; b < view[1].ncluster; b++) //if c0v[a] is inside v view we loop on u view
		{

		for (int c = 0; c < view[2].ncluster; c++) //if cluster [b] is inside u view we loop on x and y views
		    {
		    //cerco nella vista x

		    coordinate_temp = (view[0].coordinate[a] + view[1].coordinate[b]) / sqrt(2);
		    distance = fabs(view[2].coordinate[c] - coordinate_temp);

		    if (distance < clusterDistance)
			{
			if ((view[0].used[a] == 0 || view[1].used[b] == 0 || view[2].used[c] == 0)
				&& (view[2].coordinate[c] != x[npoints - 1] || (view[0].coordinate[a] - view[1].coordinate[b]) / sqrt(2) != y[npoints - 1]))
			    {

			    z[npoints] = chamberCoordiante;
			    x[npoints] = view[2].coordinate[c];
			    y[npoints] = (view[0].coordinate[a] - view[1].coordinate[b]) / sqrt(2);
			    trailing[npoints] = (view[0].trailing[a] + view[1].trailing[b] + view[2].trailing[c]) / 3;

			    npoints++;

			    view[0].used[a] = 3;
			    view[1].used[b] = 1;
			    view[2].used[c] = 1;
			    }
			else
			    {
			    if (view[0].used[a] == 4)
				view[2].used[c] = 1;

			    view[0].used[a] = 1;
			    view[1].used[b] = 1;
			    }

			}
		    }
		for (int c = 0; c < view[3].ncluster; c++) //if cluster [b] is inside u view we loop on x and y views
		    {
		    //cerco nella vista y

		    coordinate_temp = (view[0].coordinate[a] - view[1].coordinate[b]) / sqrt(2);
		    distance = fabs(view[3].coordinate[c] - coordinate_temp);

		    if (distance < clusterDistance)
			{

			if ((view[0].used[a] == 0 || view[1].used[b] == 0 || view[3].used[c] == 0)
				&& ((view[0].coordinate[a] + view[1].coordinate[b]) / sqrt(2) != x[npoints] || view[3].coordinate[c] != y[npoints]))
			    {

			    z[npoints] = chamberCoordiante;
			    x[npoints] = (view[0].coordinate[a] + view[1].coordinate[b]) / sqrt(2);
			    y[npoints] = view[3].coordinate[c];
			    trailing[npoints] = (view[0].trailing[a] + view[1].trailing[b] + view[3].trailing[c]) / 3;

			    npoints++;

			    view[0].used[a] = 4;
			    view[1].used[b] = 1;
			    view[3].used[c] = 1;
			    }
			else
			    {
			    if (view[0].used[a] == 3)
				view[3].used[c] = 1;

			    view[0].used[a] = 1;
			    view[1].used[b] = 1;
			    }

			}
		    }
		}

	    }

	}

    } /* namespace na62 */
