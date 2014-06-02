/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
 *
 *****************************************************************************/

#ifdef AM_USE_TESTS

#include <stdexcept>
#include <cstdint>
#include <iostream>

#include "bounded.h"
#include "bounded_test.h"


namespace am {
namespace num {
namespace test {


//-------------------------------------------------------------------
void bounded_correctness()
{
    using std::abs;
    constexpr auto eps = 0.01;

    auto a = num::bounded<int,num::interval<int>>{ 5, {-2,8}};
    auto b = num::bounded<int,num::interval<int>>{ 6, {-2,8}};
    auto c = num::bounded<int,num::interval<int>>{10, {-2,8}};
    auto d = num::bounded<int,num::interval<int>>{-5, {-2,8}};

    auto d0 = num::bounded<double,num::unit_interval<double>>{-0.12};
    auto d1 = num::bounded<double,num::unit_interval<double>>{0.02};
    auto d2 = num::bounded<double,num::unit_interval<double>>{0.25};
    auto d3 = num::bounded<double,num::unit_interval<double>>{0.50};
    auto d4 = num::bounded<double,num::unit_interval<double>>{0.75};
    auto d5 = num::bounded<double,num::unit_interval<double>>{0.98};
    auto d6 = num::bounded<double,num::unit_interval<double>>{1.23};

    if(!(
        //double values, compile-time bounds
        (abs(d0 - 0.00) < eps) &&
        (abs(d1 - 0.02) < eps) &&
        (abs(d2 - 0.25) < eps) &&
        (abs(d3 - 0.50) < eps) &&
        (abs(d4 - 0.75) < eps) &&
        (abs(d5 - 0.98) < eps) &&
        (abs(d6 - 1.00) < eps) &&

        (abs((d0 + d1) - 0.02) < eps) &&
        (abs((d1 + d2) - 0.27) < eps) &&
        (abs((d2 + d3) - 0.75) < eps) &&
        (abs((d3 + d4) - 1.00) < eps) &&
        (abs((d5 + d4) - 1.00) < eps) &&
        (abs((d4 - d5) - 0.00) < eps) &&

        (abs((d0 + d1 + d2) - 0.27) < eps) &&
        (abs((d3 + d4 + d5) - 1.00) < eps) &&

        //integral values, runtime bounds
        (a == 5) && (b == 6) && (c == 8) && (d == -2) &&
        ((a+b) == 11) &&
        ((a+c) == 13) &&
        ((a+d) == 3) &&
        ((a-b) == -1) &&
        ((a-c) == -3) &&
        ((a-d) == 7) &&
        ((b-a) == 1) &&
        ((b-c) == -2) &&
        ((b-d) == 8) &&
        ((c-a) == 3) &&
        ((c-b) == 2) &&
        ((c-d) == 10) &&
        ((d-a) == -7) &&
        ((d-b) == -8) &&
        ((d-c) == -10) &&
        (make_bounded(a+b, make_interval(-1,5)) == 5) &&
        (make_bounded(a+c, make_interval(-1,5)) == 5) &&
        (make_bounded(a+d, make_interval(-1,5)) == 3) &&
        (make_bounded(a-b, make_interval(-1,5)) == -1) &&
        (make_bounded(a-c, make_interval(-1,5)) == -1) &&
        (make_bounded(a-d, make_interval(-1,5)) == 5) &&
        (make_bounded(b-a, make_interval(-1,5)) == 1) &&
        (make_bounded(b-c, make_interval(-1,5)) == -1) &&
        (make_bounded(b-d, make_interval(-1,5)) == 5) &&
        (make_bounded(c-a, make_interval(-1,5)) == 3) &&
        (make_bounded(c-b, make_interval(-1,5)) == 2) &&
        (make_bounded(c-d, make_interval(-1,5)) == 5) &&
        (make_bounded(d-a, make_interval(-1,5)) == -1) &&
        (make_bounded(d-b, make_interval(-1,5)) == -1) &&
        (make_bounded(d-c, make_interval(-1,5)) == -1) &&
        true
    ))
    {
        throw std::logic_error("");
    }
}


}  // namespace test
}  // namespace num
}  // namespace am


#endif
