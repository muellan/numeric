/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 *2008-2014  André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_EQUALITY_H_
#define AM_NUMERIC_EQUALITY_H_


#include <complex>
#include <type_traits>
#include <iterator>
#include <utility>

#include "traits.h"
#include "limits.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 * NUMERICAL PRECISION HELPER FUNCTIONS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
approx_equal(const T1& a, const T2& b)
{
    return (
        (tolerance<T1>::value() < tolerance<T2>::value())
            ? ((a >= (b - tolerance<T2>::value())) &&
               (a <= (b + tolerance<T2>::value())) )
            : ((b >= (a - tolerance<T1>::value())) &&
               (b <= (a + tolerance<T1>::value())) )
        );
}

//---------------------------------------------------------
template<class T1, class T2, class T3>
inline constexpr bool
approx_equal(const T1& a, const T2& b, const T3& tolerance)
{
    return ((a >= (b - tolerance)) &&
            (a <= (b + tolerance)) );
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
approx_equal(
    const std::complex<T1>& a,
    const std::complex<T2>& b)
{
    return (
        approx_equal(a.real(), b.real()) &&
        approx_equal(a.imag(), b.imag()) );
}

//---------------------------------------------------------
template<class T1, class T2, class T3>
inline constexpr bool
approx_equal(
    const std::complex<T1>& a,
    const std::complex<T2>& b,
    const T3& tolerance)
{
    return (
        approx_equal(a.real(), b.real(), tolerance) &&
        approx_equal(a.imag(), b.imag(), tolerance) );
}



//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr bool
abs_approx_equal(const T1& a, const T2& b)
{
    using std::abs;
    return approx_equal(abs(a), abs(b));
}

//---------------------------------------------------------
template<class T1, class T2, class T3>
inline constexpr bool
abs_approx_equal(const T1& a, const T2& b, const T3& tolerance)
{
    using std::abs;
    return approx_equal(abs(a), abs(b), tolerance);
}



//-------------------------------------------------------------------
template<class T>
inline constexpr bool
approx_0(const T& a, const T& tolerance = tolerance<T>::value())
{
    return (
        (a >= (T(0) - tolerance)) &&
        (a <= (T(0) + tolerance)) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_0(const std::complex<T>& a, const T& tolerance = tolerance<T>::value())
{
    return (
        approx_0(a.real(), tolerance) &&
        approx_0(a.imag(), tolerance) );
}




//-------------------------------------------------------------------
template<class T>
inline constexpr bool
approx_1(const T& a, const T& tolerance = tolerance<T>::value())
{
    return (
        (a >= (T(1) - tolerance)) &&
        (a <= (T(1) + tolerance)) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_1(const std::complex<T>& a, const T& tolerance = tolerance<T>::value())
{
    return (
        approx_1(a.real(), tolerance) &&
        approx_0(a.imag(), tolerance) );
}



//-------------------------------------------------------------------
/// @brief compares two ranges for numerical equality
//-------------------------------------------------------------------
template<class InputIter, class T, class = typename
    std::enable_if<
        is_number<typename std::iterator_traits<InputIter>::value_type>::value &&
        is_number<T>::value>::type
>
bool
approx_equal(
    InputIter begin1, InputIter end1,
    InputIter begin2, InputIter end2,
    const T& tolerance)
{
    using std::abs;

    for(;begin1 != end1; ++begin1, ++begin2) {
        if(abs(*begin1 - *begin2) > tolerance) return false;
    }

    return true;
}


}  // namespace num
}  // namespace am


#endif
