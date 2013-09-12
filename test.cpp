/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/


#ifndef AM_NUMERIC_TESTS_H_
#define AM_NUMERIC_TESTS_H_

//#define AM_USE_TESTS
#ifdef AM_USE_TESTS

#include <iostream>

#include "angle_test.h"
#include "choice_test.h"
#include "quaternion_test.h"


namespace num {
namespace test {


//-------------------------------------------------------------------
void report_correctness()
{
	std::cout << std::boolalpha
		<< "-------------------------\n"
		<< "numeric correctness\n"
		<< "angle      " << angle_correct() << '\n'
		<< "choice     " << choice_correct() << '\n'
		<< "quaternion " << quaternion_correct() << '\n'
		<< std::flush;
}


}  // namespace test
}  // namespace num


//-------------------------------------------------------------------
int main()
{
	num::test::report_correctness();

	return 0;
}


#endif

#endif
