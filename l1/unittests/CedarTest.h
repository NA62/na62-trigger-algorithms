/*
 * CedarTest.h
 *
 *  Created on: Jan 28, 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

#ifndef L1_UNITTESTS_CEDARTEST_H_
#define L1_UNITTESTS_CEDARTEST_H_

namespace na62 {

#define BOOST_TEST_MODULE CedarTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( SomeTestName ) {
	BOOST_CHECK(true);
}

} /* namespace na62 */

#endif /* L1_UNITTESTS_CEDARTEST_H_ */
