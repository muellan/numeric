/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2015 André Müller
 *
 *****************************************************************************/

#ifdef AM_USE_TESTS

#include <iostream>
#include <stdexcept>


//-------------------------------------------------------------------
void num_angle_correctness();
void num_bounded_correctness();
void num_choice_correctness();
void num_natural_correctness();
void num_sequence_generation_correctness();


//-------------------------------------------------------------------
void numeric_correctness()
{
    try {
        num_angle_correctness();
        num_bounded_correctness();
        num_choice_correctness();
        num_natural_correctness();
        num_sequence_generation_correctness();
    }
    catch(std::exception& e) {
        std::cerr << "TESTS FAILED:\n" << e.what() << std::endl;
    }
}

#endif
