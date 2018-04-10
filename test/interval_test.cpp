/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/


#include  "../include/interval.h"
#include  "../include/limits.h"

#include <stdexcept>
#include <iostream>


using namespace am;
using namespace am::num;


//-------------------------------------------------------------------
template<class T>
void test()
{
    using std::abs;

    interval<T> i1 { T(-5), T(7)};

    if( abs(i1.min() - T(-5)) > tolerance<T> || 
        abs(i1.max() - T(7)) > tolerance<T> )
    {
        throw std::runtime_error{"min / max"};
    }

    if( abs(min(i1) - i1.min()) > tolerance<T> || 
        abs(max(i1) - i1.max()) > tolerance<T> )
    {
        throw std::runtime_error{"free-standing min / max"};
    }


    interval<T> i2 { T(4), T(-8)};

    if( abs(i2.min() - T(-8)) > tolerance<T> || 
        abs(i2.max() - T(4)) > tolerance<T> )
    {
        throw std::runtime_error{"min !< max "};
    }

    i2.expand(T(1));
    if( abs(i2.min() - T(-9)) > tolerance<T> || 
        abs(i2.max() - T(5)) > tolerance<T> )
    {
        throw std::runtime_error{"expand"};
    }

    i2.expand_include(T(8));
    if( abs(i2.min() - T(-9)) > tolerance<T> || 
        abs(i2.max() - T(8)) > tolerance<T> )
    {
        throw std::runtime_error{"expand_include max"};
    }

    i2.expand_include(T(-10));
    if( abs(i2.min() - T(-10)) > tolerance<T> || 
        abs(i2.max() - T(8)) > tolerance<T> )
    {
        throw std::runtime_error{"expand_include min"};
    }

    i2.shrink_exclude(T(8), T(1));

    if( abs(i2.min() - T(-10)) > tolerance<T> || 
        abs(i2.max() - T(7)) > tolerance<T> )
    {
        throw std::runtime_error{"shrink_exclude max"};
    }

    i2.shrink_exclude(T(-7), T(1));
    if( abs(i2.min() - T(-6)) > tolerance<T> || 
        abs(i2.max() - T(7)) > tolerance<T> )
    {
        throw std::runtime_error{"shrink_exclude min"};
    }

    if( abs(i2.width() - T(13)) > tolerance<T>) {
        throw std::runtime_error{"width"};
    }

    if( abs(i2.half_width() - floating_point_t<T>(6.5)) > tolerance<T>) {
        throw std::runtime_error{"half_width"};
    }

    if( abs(i2.center() - floating_point_t<T>(0.5)) > tolerance<T>) {
        throw std::runtime_error{"center"};
    }

    i2.clear();
    if( !i2.empty() ||
        abs(i2.min()) > tolerance<T> ||
        abs(i2.max()) > tolerance<T>) 
    {
        throw std::runtime_error{"center"};
    }
    //assign
    i2.assign(-3,4);
    if( abs(i2.min() - T(-3)) > tolerance<T> || 
        abs(i2.max() - T(4)) > tolerance<T> )
    {
        throw std::runtime_error{"assign #1"};
    }

    i2.assign(3,-4);
    if( abs(i2.min() - T(-4)) > tolerance<T> || 
        abs(i2.max() - T(3)) > tolerance<T> )
    {
        throw std::runtime_error{"assign #2"};
    }

    //contains
    if( !i2.contains(-1) || !i2.contains( 0) || !i2.contains(-1) ||
        !i2.contains(-4) || !i2.contains( 3) ||
        !i2.contains(i2.min()) || !i2.contains(i2.max()) ||
         i2.contains(-5) || i2.contains( 5) 
        ) 
    {
        throw std::runtime_error{"contains"};
    }

    if( !i2.intersects(interval<T>{0,0}) ||
        !i2.intersects(interval<T>{-1,1}) ||
        !i2.intersects(interval<T>{-10,2}) ||
        !i2.intersects(interval<T>{2,10}) ||
        !i2.intersects(interval<T>{-10,10}) ||
         i2.intersects(interval<T>{-10,-5}) ||
         i2.intersects(interval<T>{5,10})
        ) 
    {
        throw std::runtime_error{"intersects"};
    }

    i2 += interval<T>{2, 3};
    if( abs(i2.min() - T(-2)) > tolerance<T> || 
        abs(i2.max() - T(6)) > tolerance<T> )
    {
        throw std::runtime_error{"operator +="};
    }

    i2 -= interval<T>{3, 4};
    if( abs(i2.min() - T(-6)) > tolerance<T> || 
        abs(i2.max() - T(3)) > tolerance<T> )
    {
        throw std::runtime_error{"operator -="};
    }

    i2 *= interval<T>{5, 6};
    if( abs(i2.min() - T(-36)) > tolerance<T> || 
        abs(i2.max() - T(18)) > tolerance<T> )
    {
        throw std::runtime_error{"operator *="};
    }

    i2 /= interval<T>{5, 6};
    if( abs(i2.min() - T(-7.2)) > tolerance<T> || 
        abs(i2.max() - T(3.6)) > tolerance<T> )
    {
        throw std::runtime_error{"operator /="};
    }

}



//-------------------------------------------------------------------
int main()
{
    try {
        // test<char>();
        // test<short int>();
        test<int>();
        test<long int>();
        test<long long int>();

        test<float>();
        test<double>();
        test<long double>();
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
