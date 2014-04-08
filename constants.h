/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_CONSTANTS_H_
#define AM_NUMERIC_CONSTANTS_H_


#include <cstdint>


namespace am {

namespace num {


/*****************************************************************************
 *
 *
 * TYPES
 *
 *
 *****************************************************************************/

//------------------------------------------------------------------
/// @brief default floating point
//------------------------------------------------------------------
#ifdef AM_EXTENDED_DOUBLE_PRECISION
    using real_t = long double;
#else
    #ifdef AM_DOUBLE_PRECISION
        using real_t = double;
    #else
        using real_t = float;
    #endif
#endif



//-------------------------------------------------------------------
/// @brief default dimension type
//-------------------------------------------------------------------
#ifdef AM_DIMENSIONS_HUGE
    using dims_t = std::int_fast32_t;
#else
    #ifdef AM_DIMENSIONS_MANY
        using dims_t = std::int_fast16_t;
    #else
        using dims_t = std::int_fast8_t;
    #endif
#endif






/*****************************************************************************
 *
 *
 * CONSTANTS
 *
 *
 *****************************************************************************/

//---------------------------------------------------------------
/// @brief mathematical constants
//  TODO C++14 replace with variable templates
//---------------------------------------------------------------
constexpr long double pi = 3.141592653589793238462643383279500;

constexpr long double euler_e  = 2.718281828459045235360287471352663;

constexpr long double log2_e   = 1.4426950408889634074;
constexpr long double log10_e  = 0.43429448190325182765;
constexpr long double ln2      = 0.69314718055994530942;
constexpr long double ln10     = 2.30258509299404568402;

constexpr long double sqrt2    = 1.41421356237309504880;
constexpr long double sqrt1_2  = 0.70710678118654752440;



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
