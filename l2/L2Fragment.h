/*
 * L2Fragment.h
 *
 *  Created on: Jun 16, 2015
 *      Author: Angela Romano (angela.romano@cern.ch)
 */

#pragma once
#ifndef L2Fragment_H_
#define L2Fragment_H_

namespace na62 {


struct L2_BLOCK {
	uint32_t triggerword;
//	uint16_t globaldownscaling;
//	uint16_t globalreduction;
};

struct L2GLOBAL {
	uint32_t x;
	uint32_t y;
	uint32_t z;
};

struct L2MASK {
	uint32_t xx;
	uint32_t yy;
	uint32_t zz;
};

}


#endif    /*  L2Fragment_H */
