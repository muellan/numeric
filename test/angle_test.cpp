/*****************************************************************************
 *
 * AM utilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/


#include  "../include/angle.h"
#include  "../include/limits.h"

#include <stdexcept>
#include <iostream>


using namespace am;
using namespace am::num;


//-------------------------------------------------------------------
template<int a>
struct tester {
    static constexpr int get() noexcept {return a; }
};


//-------------------------------------------------------------------
void initialization()
{
    using namespace am::num::literals;
    using std::abs;

    auto d0 = degd{15};
    degf d1 {45.0f};
    auto d2 = 90.0_deg;  //via user-defined literal
    auto d3 = 1_pi_rad;  //via user-defined literal + implicit angle conversion
    auto d4 = radf{4.5f};
    auto d5 = radd{double(pi)};

    if(! (
        (abs(degrees_cast<double>(d0) - 15.0)         < tolerance<double>::value()) &&
        (abs(degrees_cast<double>(d1) - 45.0)         < double(tolerance<float>::value())) &&
        (abs(degrees_cast<double>(d2) - 90.0)         < tolerance<double>::value()) &&
        (abs(radians_cast<double>(d3) - double(1_pi)) < tolerance<double>::value()) &&
        (abs(radians_cast<double>(d4) - 4.5)          < double(tolerance<float>::value())) &&
        (abs(radians_cast<double>(d5) - double(pi))   < tolerance<double>::value()) ))
    {
        throw std::logic_error("am::num::angle init");
    }
}


//-------------------------------------------------------------------
void conversion()
{
    using namespace am::num::literals;
    using std::abs;

    auto a = deg{90};
    auto b = rad{float(0.5_pi)};

    if(!((abs(radians_cast<double>(degi{180})            - double(1_pi)) < tolerance<double>::value()) &&
         (abs(degrees_cast<double>(rad{float(pi)/4})     - 45)     < tolerance<double>::value()) &&
         (abs(gons_cast<double>(rad{float(pi)/4})        - 50)     < tolerance<double>::value()) &&
         (abs(angle_cast<num::radians_turn<float>>(a)    - float(0.5_pi)) < tolerance<float>::value()) &&
         (abs(angle_cast<num::degrees_turn<double>>(b)   - 90)     < tolerance<double>::value()) &&
         (abs(angle_cast<num::gons_turn<long double>>(b) - 100)    < tolerance<long double>::value()) ))
    {
        throw std::logic_error("am::num::angle conversion");
    }
}


//-------------------------------------------------------------------
void arithmetic()
{
    using namespace am::num::literals;
    using std::abs;
    constexpr auto eps = tolerance<double>::value();

    auto a = degi{20};
    auto b = degi{30};
    auto c = degi{a+b};
    auto d = deg{deg{270} - rad{real_t(pi)/2}};
    auto e = (90_deg + rad{real_t(0.5_pi)} + 0.5_pi_rad);

    if(!((degrees_cast<int>(c) == 50) &&
         (abs(degrees_cast<double>(d) - 180) < eps) &&
         (abs(degrees_cast<double>(e) - 270) < eps)    ))
    {
        throw std::logic_error("am::num::angle arithmetic");
    }
}


//-------------------------------------------------------------------
void modification()
{
    //integral angles are incrementable
    degi di{65};
    ++di;
    di++;

    //increment/decrement not possible with non-integral domain types
//    auto d = ++deg{40};

    if((degrees_cast<int>(di) != 67)) {
        throw std::logic_error("am::num::angle modification");
    }
}


//-------------------------------------------------------------------
void static_eval()
{
    //compile-time evaluation
    constexpr auto ca = degi{45};

    if( (tester<degrees_cast<int>(ca)>::get() != 45) ||
        (tester<degrees_cast<int>(degi{70})>::get() != 70) )
    {
        throw std::logic_error("am::num::angle static eval");
    }
}


//-------------------------------------------------------------------
void functions()
{
    using std::abs;
    constexpr auto eps = tolerance<float>::value();

    if(!(
        (abs(cos(rad(0)) -  1) < eps) &&
        (abs(cos(rad{real_t(pi)/2}) ) < eps) &&
        (abs(cos(deg(0)) -  1) < eps) &&
        (abs(cos(deg(90)) ) < eps) &&
        (abs(cos(deg(30)) - real_t(0.8660254037844386)) < eps) &&
        (abs(cos(rad{real_t(pi)/3}) - real_t(0.5)) < eps) &&
        (abs(sin(deg(30)) - real_t(0.5)) < eps) &&
        (abs(sin(rad{real_t(pi)/3}) - real_t(0.8660254037844386)) < eps) &&
        (abs(sin(rad(0)) ) < eps) &&
        (abs(sin(rad{real_t(pi)/2}) - 1) < eps) &&
        (abs(sin(deg(0)) ) < eps) &&
        (abs(sin(deg(90)) - 1) < eps) &&
        (abs(degrees_cast<real_t>(rad{real_t(pi)/2}) - 90) < eps) ))
    {
        throw std::logic_error("am::num::angle trigonometric functions");
    }

}


//-------------------------------------------------------------------
void comparison()
{
    //comparison: == and != only possible for integral angles
    if(!(
        (degi{30} == degi{30}) &&
        (degi{30} != degi{40}) &&
        (deg(180) >  rad{real_t(pi)/4}) &&
        (deg(30)  <  rad{real_t(pi)}) &&
        (rad{real_t(pi)}   >= deg(45)) &&
        (rad{2*real_t(pi)} <= deg(720)) ))
    {
        throw std::logic_error("am::num::angle comparison");
    }
}



//-------------------------------------------------------------------
int main()
{
    try {
        initialization();
        static_eval();
        conversion();
        arithmetic();
        modification();
        functions();
        comparison();
    }
    catch(std::exception& e) {
        std::cerr << e.what();
        return 1;
    }
}
