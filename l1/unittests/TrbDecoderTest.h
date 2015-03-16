/*
 * CedarTest.h
 *
 *  Created on: Jan 28, 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#pragma once
#ifndef L1_UNITTESTS_TRBDECODER_H_
#define L1_UNITTESTS_TRBDECODER_H_

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <options/Logging.h>

#include "../data_decoder/TrbDecoder.h"

namespace na62 {

#define BOOST_TEST_MODULE CedarTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TesTrbDecoder ) {

	uint flag = 1;
	for (uint i = 1; i != 9; i++) {
		uint val = TrbDecoder::calculateNumberOfFPGAs(flag);

		BOOST_CHECK(val == i);
		flag *= 2;
	}
}

} /* namespace na62 */

#endif /* L1_UNITTESTS_TRBDECODER_H_ */
