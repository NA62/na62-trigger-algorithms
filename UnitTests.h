/*
 * UnitTests.h
 *
 *  Created on: Jan 28, 2015
 *      Author: Jonas Kunze (kunze.jonas@gmail.com)
 */

/*
 * Include your unit test files here. If they look like following they will be automatically executed:
 *
 #ifndef MY_UNIT_TEST_H_
 #define MY_UNIT_TEST_H_

 #define BOOST_TEST_MODULE MyTest
 #include <boost/test/unit_test.hpp>

 BOOST_AUTO_TEST_CASE( my_test ) {
 SomeObject obj;
 BOOST_CHECK(obj.method(123)==567);
 }
 #endif

 More information: http://www.boost.org/doc/libs/1_44_0/libs/test/doc/html/tutorials/new-year-resolution.html
 */

#ifndef UNITTESTS_H_
#define UNITTESTS_H_

#include "common/unittests/DecoderHandlerTest.h"
#include "common/unittests/TrbFragmentDecoderTest.h"
#include "l1/unittests/CedarTest.h"

#endif /* UNITTESTS_H_ */
