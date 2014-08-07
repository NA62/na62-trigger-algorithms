/*
 * StrawDetectorParameters.h
 *
 *  Created on: Jul 30, 2014
 *      Author: Thomas Hancock (Thomas.hancock@cern.ch)
 */
 
#pragma once
#ifndef STRAWDETECTORPARAMETERS_H_
#define STRAWDETECTORPARAMETERS_H_

namespace na62 {
namespace strawparameters {

const int NO_STRAWS_PER_LAYER = 112;
const double STRAW_SPACING = 17.6; // mm
const double LAYER_SPACING = 11.0; // mm
const double LAYER_DISPLACEMENT = 8.8; // mm
const double HALF_VIEW_SPACING = 26.0; // mm
const double HALF_VIEW_DISPLACEMENT = 4.4; // mm

const double STRAW_RADIUS = 4.911; // mm
const double STRAW_LENGTH = 2100; // mm 

const double UV_AND_XY_SPACING = 57.0; // mm
const double VX_SPACING = 233.0; // mm

const double CENTRAL_GAP_WIDTH = 120.0; //mm <- Warning: The technical document says ~12cm. This will need updating with a more accurate measurement.
const double CENTRAL_GAP_DISPLACEMENT[4][4] = { // [View][Chamber]
	{101.2, 114.4, 92.4, 52.8, }, // X view
	{0.0, 0.0, 0.0, 0.0, }, // Y view
	{70.4, 79.2, 66.0, 39.6, }, // V view
	{70.4, 79.2, 66.0, 39.6, } // U view
};

const double PLANE_INDENTS[4] = {0.0, 8.8, 13.2, 4.4}; // Warning: Need to check the array element corresponds to the correct number in the MC

const double CHAMBERS_ONE_AND_TWO_SPACING = 198.0699 - 183.7049; // m. Calculated from the end of 1 to the end of 2.

} /* namespace strawparameters */
} /* namespace na62 */

#endif /* STRAWDETECTORPARAMETERS_H_ */