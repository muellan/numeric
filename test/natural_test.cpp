/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/


#include  "../include/natural.h"

#include <stdexcept>
#include <cstdint>
#include <iostream>


using namespace am;
using namespace am::num;


//-------------------------------------------------------------------
void natural_init()
{
    auto c1 = natural<std::int_least8_t>{std::int_least8_t(0)};
    auto c2 = natural<std::int_least16_t>{std::int_least16_t(1)};
    auto c3 = natural<std::int_least32_t>{5};
    auto c4 = natural<std::int_least64_t>{8};
    auto c5 = natural<std::intmax_t>{123};
    auto c6 = natural<long long int>{12345678};

    if ((intmax_t(c1) != 0) ||
        (intmax_t(c2) != 1) ||
        (intmax_t(c3) != 5) ||
        (intmax_t(c4) != 8) ||
        (intmax_t(c5) != 123) ||
        (intmax_t(c6) != 12345678) )
    {
        throw std::logic_error("am::num::natural init");
    }
}



//-------------------------------------------------------------------
void natural_arithmetic()
{
    auto a = natural<int>{0};
    auto b = natural<int>{1};
    auto c = natural<long long>{5};
    auto d = natural<long long int>{10};

    auto e = natural<int>::infinity();
    auto f = natural<long long int>::infinity();

    if(!(
        ((a * a) == 0) &&     ((b * a) == 0) &&     ((c * a) == 0) &&
        ((a * b) == 0) &&     ((b * b) == 1) &&     ((c * b) == 5) &&
        ((a * c) == 0) &&     ((b * c) == 5) &&     ((c * c) == 25) &&
        ((a * d) == 0) &&     ((b * d) == 10) &&    ((c * d) == 50) &&
        ((a * e) == 0) &&     isinf(b * e) &&       isinf(c * e) &&
        ((a * f) == 0) &&     isinf(b * f) &&       isinf(c * f) &&

        ((d * a) == 0) &&     ((e * a) == 0) &&      ((f * a) == 0) &&
        ((d * b) == 10) &&    isinf(e * b) &&        isinf(f * b) &&
        ((d * c) == 50) &&    isinf(e * c) &&        isinf(f * c) &&
        ((d * d) == 100) &&   isinf(e * d) &&        isinf(f * d) &&
        isinf(d * e) &&       isinf(e * e) &&        isinf(f * e) &&
        isinf(d * f) &&       isinf(e * f) &&        isinf(f * f) &&

        ((a - a) == 0) &&     ((b - a) == 1) &&      ((c - a) == 5) &&
        ((a - b) == 0) &&     ((b - b) == 0) &&      ((c - b) == 4) &&
        ((a - c) == 0) &&     ((b - c) == 0) &&      ((c - c) == 0) &&
        ((a - d) == 0) &&     ((b - d) == 0) &&      ((c - d) == 0) &&
        ((a - e) == 0) &&     ((b - e) == 0) &&      ((c - e) == 0) &&
        ((a - f) == 0) &&     ((b - f) == 0) &&      ((c - f) == 0) &&

        ((d - a) == 10) &&    isinf(e - a) &&        isinf(f - a) &&
        ((d - b) == 9) &&     isinf(e - b) &&        isinf(f - b) &&
        ((d - c) == 5) &&     isinf(e - c) &&        isinf(f - c) &&
        ((d - d) == 0) &&     isinf(e - d) &&        isinf(f - d) &&
        ((d - e) == 0) &&     ((e - e) == 0) &&      ((f - e) == 0) &&
        ((d - f) == 0) &&     ((e - f) == 0) &&      ((f - f) == 0) &&

        ((a + a) == 0) &&     ((b + a) == 1) &&      ((c + a) == 5) &&
        ((a + b) == 1) &&     ((b + b) == 2) &&      ((c + b) == 6) &&
        ((a + c) == 5) &&     ((b + c) == 6) &&      ((c + c) == 10) &&
        ((a + d) == 10) &&    ((b + d) == 11) &&     ((c + d) == 15) &&
        isinf(a + e) &&       isinf(b + e) &&        isinf(c + e) &&
        isinf(a + f) &&       isinf(b + f) &&        isinf(c + f) &&

        ((d + a) == 10) &&    isinf(e + a) &&        isinf(f + a) &&
        ((d + b) == 11) &&    isinf(e + b) &&        isinf(f + b) &&
        ((d + c) == 15) &&    isinf(e + c) &&        isinf(f + c) &&
        ((d + d) == 20) &&    isinf(e + d) &&        isinf(f + d) &&
        isinf(d + e) &&       isinf(e + e) &&        isinf(f + e) &&
        isinf(d + f) &&       isinf(e + f) &&        isinf(f + f) &&

        true ))
    {
        throw std::logic_error("am::num::natural arithmetic");
    }
}



//-------------------------------------------------------------------
void natural_comparison()
{
    auto a = natural<int>{0};
    auto b = natural<int>{1};
    auto c = natural<int>{5};
    auto d = natural<int>::infinity();
    auto e = natural<long long int>::infinity();

    //== != <= < > >=
    if(!(
        (a == a) && (b == b) && (c == c) && (d == d) && (e == e)
        &&
        (a != b) && (a != c) && (a != d) && (a != e) &&
        (b != a) && (b != c) && (b != d) && (b != e) &&
        (c != a) && (c != b) && (c != d) && (c != e) &&
        (d != a) && (d != b) && (d != c) && (d == e) &&
        (e != a) && (e != b) && (e != c) && (e == d)
        &&
        (a <  b) && (a <  c) && (a <  d) && (a <  e) &&
        (a <= a) && (a <= b) && (a <= c) && (a <= d) && (a <= e)
        &&
        (b <  c) && (b <  d) && (b <  e) &&
        (b <= b) && (b <= c) && (b <= d) && (b <= e)
        &&
        (c <  d) && (c <  e) &&
        (c <= c) && (c <= d) && (c <= e)
        &&
        (d <= d) && (d <= e)
        &&
        (e >  a) && (e >  b) && (e >  c) &&
        (e >= a) && (e >= b) && (e >= c) && (e >= d) && (e >= e)
        &&
        (d >  a) && (d >  b) && (d >  c) &&
        (d >= a) && (d >= b) && (d >= c) && (d >= d) && (d >= e)
        &&
        (c >  a) && (c >  b) &&
        (c >= a) && (c >= b) && (c >= c)
        &&
        (b >  a) &&
        (b >= a) && (b >= b)
        &&
        (a >= a)
    ))
    {
        throw std::logic_error("am::num::natural comparison");
    }
}



//-------------------------------------------------------------------
int main()
{
    try {
        natural_init();
        natural_arithmetic();
        natural_comparison();
    }
    catch(std::exception& e) {
        std::cerr << e.what();
        return 1;
    }
}
