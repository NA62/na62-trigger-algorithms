/*
 * CedarTest.h
 *
 *  Created on: Jan 28, 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#pragma once

#include "../decoding/Decoder.h"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <options/Logging.h>


namespace na62 {

#define BOOST_TEST_MODULE CedarTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( TestDecoder ) {
/*
	Decoder decoder();
	decoder.decodeCEDARData();
	const auto& data = decoder.getDecodedCEDARData(0);
	BOOST_CHECK(data.getTimes() != nullptr);
*/

}

} /* namespace na62 */

