/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/


#ifndef AM_NUM_NUMERIC_TESTS_H_
#define AM_NUM_NUMERIC_TESTS_H_

#ifdef USE_TESTS

#include "angle_test.h"
#include "quaternion_test.h"


namespace num {
namespace test {


//-------------------------------------------------------------------
/// TODO add dual<T> tests
void report_correctness()
{
	std::cout << std::boolalpha
		<< "angle      " << test::angle_correct() << '\n'
//		<< "dual       " << test::dual_correct() << '\n'
		<< "quaternion " << test::quaternion_correct() << '\n'
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
