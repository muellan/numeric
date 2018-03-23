/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#include  "../include/choice.h"

#include <stdexcept>
#include <iostream>


using namespace am;
using namespace am::num;


//-------------------------------------------------------------------
void initialization()
{
    auto c1 = choice<std::int8_t,8>{0};
    auto c2 = choice<std::int8_t,8>{1};
    auto c3 = choice<std::int8_t,8>{5};
    auto c4 = choice<std::int8_t,8>{8};
    auto c5 = choice<std::int8_t,8>{123};

    if ((int(c1) != 0) ||
        (int(c2) != 1) ||
        (int(c3) != 5) ||
        (int(c4) != 0) ||
        (int(c5) != 3)    )
    {
        throw std::logic_error("am::num::choice init");
    }
}


//-------------------------------------------------------------------
void arithmetic()
{
    auto c1 = choice<std::int8_t,8>{2};

    auto c2 = choice<std::int8_t,11>{0};
    c2 += 123456;
    auto c3 = c2;
    c3 -= 1123;

    if(!(
        (int(c1 +    1) == 3) && (int(    1 + c1) == 3) &&
        (int(c1 +    2) == 4) && (int(    2 + c1) == 4) &&
        (int(c1 +    8) == 2) && (int(    8 + c1) == 2) &&
        (int(c1 + 1233) == 3) && (int( 1233 + c1) == 3) &&
        (int(c1 -    5) == 5) && (int(-5    + c1) == 5) &&
        (int(c1 - 2323) == 7) && (int(-2323 + c1) == 7)
        &&
        (int(c1 -    1) == 1) && (int(    1 - c1) == 7) &&
        (int(c1 -    2) == 0) && (int(    2 - c1) == 0) &&
        (int(c1 -    8) == 2) && (int(    8 - c1) == 6) &&
        (int(c1 - 1233) == 1) && (int( 1233 - c1) == 7) &&
        (int(c1 +    5) == 7) && (int(-5    - c1) == 1) &&
        (int(c1 + 2323) == 5) && (int(-2323 - c1) == 3)
        &&
        (int(c1 *     1) == 2) && (int(    1 * c1) == 2) &&
        (int(c1 *     2) == 4) && (int(    2 * c1) == 4) &&
        (int(c1 *     8) == 0) && (int(    8 * c1) == 0) &&
        (int(c1 *  1233) == 2) && (int( 1233 * c1) == 2) &&
        (int(c1 *    -5) == 6) && (int(-5    * c1) == 6) &&
        (int(c1 * -2323) == 2) && (int(-2323 * c1) == 2)
        &&
        (int(c2) == 3) &&
        (int(c3) == 2) ) )
    {
        throw std::logic_error("am::num::choice arithmetic");
    }
}



//-------------------------------------------------------------------
int main()
{
    try {
        initialization();
        arithmetic();
    }
    catch(std::exception& e) {
        std::cerr << e.what();
        return 1;
    }
}
