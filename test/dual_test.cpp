/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/


#include  "../include/dual.h"

#include <stdexcept>
#include <cstdint>
#include <iostream>


using namespace am;
using namespace am::num;


//-------------------------------------------------------------------
template<class T>
void test()
{
    using namespace am;
    using namespace am::num;

    using std::abs;

    dual<T> d1 {T(0), T(0)};
    if( abs( d1.imag() - T(0) ) > tolerance<T> ||
        abs( d1.real() - T(0) ) > tolerance<T> )
    {
        throw std::runtime_error{"construction #1"};
    }

    dual<T> d2 {T(-2), T(5)};
    if( abs( d2.imag() - T(5) ) > tolerance<T> ||
        abs( d2.real() - T(-2) ) > tolerance<T> )
    {
        throw std::runtime_error{"construction #2"};
    }

    dual<T> d3 {T(-1.23), T(3.45)};
    if( abs( d3.imag() - T(3.45) ) > tolerance<T> ||
        abs( d3.real() - T(-1.23) ) > tolerance<T> )
    {
        throw std::runtime_error{"construction #3"};
    }
    
}



//-------------------------------------------------------------------
int main()
{
    using namespace am::num;

    try {
        test<float>();
        test<double>();
        test<long double>();
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}

