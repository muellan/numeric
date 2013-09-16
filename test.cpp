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
//#include "angle.h"
//#include "biquaternion.h"
#include "choice_test.h"
//#include "choice.h"
//#include "concepts.h"
//#include "constants.h"
//#include "dual_quaternion.h"
//#include "dual.h"
//#include "equality.h"
//#include "interval.h"
//#include "narrowing.h"
#include "quaternion_test.h"
//#include "quaternion.h"
//#include "rational.h"
//#include "scomplex.h"
//#include "split_biquaternion.h"


namespace am {

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
} //namespace num
} //namespace am


//-------------------------------------------------------------------
int main()
{
	am::num::test::report_correctness();

	return 0;
}


#endif

#endif
