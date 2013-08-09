/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/


#ifndef AM_NUMERIC_EQUALITY_H_
#define AM_NUMERIC_EQUALITY_H_


#include "constants.h"


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
numerically_equal(const T1& a, const T2& b)
{
	return (
		(epsilon<T1>::value < epsilon<T2>::value)
			? ((a > (b - epsilon<T2>::value)) &&
			   (a < (b + epsilon<T2>::value)) )
			: ((b > (a - epsilon<T1>::value)) &&
			   (b < (a + epsilon<T1>::value)) )
		);
}

//-------------------------------------------------------------------
template<class T>
inline constexpr bool
is_numerically_0(const T& a)
{
	return (
		(a > (T(0) - epsilon<T>::value)) &&
		(a < (T(0) + epsilon<T>::value)) );
}

//-------------------------------------------------------------------
template<class T>
inline constexpr bool
is_numerically_1(const T& a)
{
	return (
		(a > (T(1) - epsilon<T>::value)) &&
		(a < (T(1) + epsilon<T>::value)) );
}


}  // namespace num


#endif
