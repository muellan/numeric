/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2015 André Müller
 *
 *****************************************************************************/

#include <iostream>
#include <stdexcept>

#include "tests.h"


//-------------------------------------------------------------------
void numeric_correctness()
{
    try {
        test::num_angle_correctness();
        test::num_bounded_correctness();
        test::num_choice_correctness();
        test::num_natural_correctness();
        test::num_sequence_generation_correctness();

    }
    catch(std::exception& e) {
        std::cerr << "TESTS FAILED:\n" << e.what() << std::endl;
    }
}


//-------------------------------------------------------------------
int main()
{
    numeric_correctness();
}
