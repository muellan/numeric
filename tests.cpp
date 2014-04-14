/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 *2008-2014  André Müller
 *
 *****************************************************************************/

#ifdef AM_USE_TESTS

#include <iostream>
#include <stdexcept>
#include <random>

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


    try {
        num::test::sequence_generation_correctness();
    }
    catch(std::exception& e) {
        std::cerr << "FAIL\n" << "am::num sequence generation\n" << e.what() << std::endl;
    }


//    try {
//        num::test::rational_correctness();
//    }
//    catch(std::exception& e) {
//        std::cerr << "FAIL\n" << "am::num::rational\n" << e.what() << std::endl;
//    }

//    try {
//        num::test::dual_correctness();
//    }
//    catch(std::exception& e) {
//        std::cerr << "FAIL\n" << "am::num::dual\n" << e.what() << std::endl;
//    }

//    try {
//        num::test::scomplex_correctness();
//    }
//    catch(std::exception& e) {
//        std::cerr << "FAIL\n" << "am::num::scomplex\n" << e.what() << std::endl;
//    }



}


}  // namespace test
}  // namespace am


#endif
