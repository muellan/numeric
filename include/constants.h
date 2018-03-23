/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AMLIB_NUMERIC_CONSTANTS_H_
#define AMLIB_NUMERIC_CONSTANTS_H_


#include <cstdint>


namespace am {
namespace num {


/*****************************************************************************
 *
 * TYPES
 *
 *****************************************************************************/

//------------------------------------------------------------------
/// @brief default floating point
//------------------------------------------------------------------
#ifdef AM_NUMERIC_PRECISION_LONG_DOUBLE
    using real_t = long double;
#else
    #ifdef AM_NUMERIC_PRECISION_DOUBLE
        using real_t = double;
    #else
        using real_t = float;
    #endif
#endif






/*****************************************************************************
 *
 * CONSTANTS
 *
 *****************************************************************************/

//---------------------------------------------------------------
/// @brief mathematical constants
//  TODO C++14 replace with variable templates ?
//---------------------------------------------------------------
constexpr long double pi      = static_cast<long double>(3.141592653589793238462643383279500);

constexpr long double euler_e = static_cast<long double>(2.718281828459045235360287471352663);

constexpr long double log2_e  = static_cast<long double>(1.4426950408889634074);
constexpr long double log10_e = static_cast<long double>(0.43429448190325182765);
constexpr long double ln2     = static_cast<long double>(0.69314718055994530942);
constexpr long double ln10    = static_cast<long double>(2.30258509299404568402);

constexpr long double sqrt2   = static_cast<long double>(1.41421356237309504880);
constexpr long double sqrt1_2 = static_cast<long double>(0.70710678118654752440);



namespace literals {

//---------------------------------------------------------
/// @brief pi literal
//---------------------------------------------------------
constexpr long double operator "" _pi(long double x)
{
    return (x * pi);
}
constexpr long double operator "" _pi(unsigned long long int x)
{
    return (x * pi);
}


//---------------------------------------------------------
/// @brief euler e literal
//---------------------------------------------------------
constexpr long double operator "" _e(long double x)
{
    return (x * euler_e);
}
//---------------------------------------------------------
constexpr long double operator "" _e(unsigned long long int x)
{
    return (x * euler_e);
}

}  // namespace literals


}  // namespace num
}  // namespace am


#endif
