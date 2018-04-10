/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/


#include  "../include/rational.h"
#include  "../include/limits.h"

#include <stdexcept>
#include <iostream>


using namespace am;
using namespace am::num;


//-------------------------------------------------------------------
template<class T>
void test()
{
    rational<T> r1 {1, 4};
    rational<T> r2 {1, 8};

    auto r3 = r1 + r2;
    if(r3.numer() != 12 || r3.denom() != 32) {
        throw std::runtime_error{"addition"};
    }

    auto r4 = r1 - r2;
    if(r4.numer() != 4 || r4.denom() != 32) {
        throw std::runtime_error{"subtraction"};
    }

    auto r5 = r1 * r2;
    if(r5.numer() != 1 || r5.denom() != 32) {
        throw std::runtime_error{"multiplication"};
    }

    auto r6 = r1 / r2;
    if(r6.numer() != 8 || r6.denom() != 4) {
        throw std::runtime_error{"division"};
    }


    r3.normalize();
    r4.normalize();
    r5.normalize();
    r6.normalize();
    
    if( r3.numer() != 3 || r3.denom() !=  8 ||
        r4.numer() != 1 || r4.denom() !=  8 ||
        r5.numer() != 1 || r5.denom() != 32 ||
        r6.numer() != 2 || r6.denom() !=  1 ) 
    {
        throw std::runtime_error{"normalization"};
    }
}



//-------------------------------------------------------------------
int main()
{
    try {
        test<short>();
        test<int>();
        test<long int>();
        test<long long int>();
    }
    catch(std::exception& e) {
        std::cerr << e.what();
        return 1;
    }
}
