/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifdef AM_USE_TESTS

#include <iostream>
#include <stdexcept>

#include "tests.h"


namespace am {
namespace test {


//-------------------------------------------------------------------
void numeric_correctness()
{
	try {
		num::test::angle_correctness();
	}
	catch(std::exception& e) {
		std::cerr << "FAIL\n" << "am::num::angle\n" << e.what() << std::endl;
	}

	try {
		num::test::choice_correctness();
	}
	catch(std::exception& e) {
		std::cerr << "FAIL\n" << "am::num::choice\n" << e.what() << std::endl;
	}


	try {
		num::test::quaternion_correctness();
	}
	catch(std::exception& e) {
		std::cerr << "FAIL\n" << "am::num::quaternion\n" << e.what() << std::endl;
	}


}


}  // namespace test
}  // namespace am


#endif
