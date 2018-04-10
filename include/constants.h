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

template<class T>
constexpr T pi = static_cast<T>(3.141592653589793238462643383279500);

template<class T>
constexpr T euler_e = static_cast<T>(2.718281828459045235360287471352663);

template<class T>
constexpr T log2_e = static_cast<T>(1.4426950408889634074);

template<class T>
constexpr T log10_e = static_cast<T>(0.43429448190325182765);

template<class T>
constexpr T ln2 = static_cast<T>(0.69314718055994530942);

template<class T>
constexpr T ln10 = static_cast<T>(2.30258509299404568402);

template<class T>
constexpr T sqrt2 = static_cast<T>(1.41421356237309504880);

template<class T>
constexpr T sqrt1_2 = static_cast<T>(0.70710678118654752440);



namespace literals {

//---------------------------------------------------------
/// @brief pi literal
//---------------------------------------------------------
constexpr long double operator "" _pi(long double x)
{
    return (x * pi<long double>);
}
constexpr long double operator "" _pi(unsigned long long int x)
{
    return (x * pi<long double>);
}


//---------------------------------------------------------
/// @brief euler e literal
//---------------------------------------------------------
constexpr long double operator "" _e(long double x)
{
    return (x * euler_e<long double>);
}
//---------------------------------------------------------
constexpr long double operator "" _e(unsigned long long int x)
{
    return (x * euler_e<long double>);
}

}  // namespace literals


}  // namespace num
}  // namespace am


#endif
