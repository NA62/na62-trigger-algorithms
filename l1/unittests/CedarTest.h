/*
 * CedarTest.h
 *
 *  Created on: Jan 28, 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#ifndef L1_UNITTESTS_CEDARTEST_H_
#define L1_UNITTESTS_CEDARTEST_H_

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <options/Logging.h>

#include "../KtagAlgo.h"

namespace na62 {

#define BOOST_TEST_MODULE CedarTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestSearchPMT ) {

	uint NTel62 = 6;
	uint Nfpga = 4;

	uint result = 0;
	uint count = 0;

	for (uint iTel = 0; iTel < NTel62; iTel++) {
		for (uint ifpga = 0; ifpga < Nfpga; ifpga++) {
			if (count && !(count % 3))
				result++;
			BOOST_CHECK(KtagAlgo::calculateSector(iTel, ifpga) == result);
			count++;
		}
	}
}

} /* namespace na62 */

#endif /* L1_UNITTESTS_CEDARTEST_H_ */
