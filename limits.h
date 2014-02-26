/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_LIMITS_H_
#define AM_NUMERIC_LIMITS_H_


#include <limits>

#include "traits.h"


namespace am {

namespace num {


/*****************************************************************************
 *
 *
 * MAXIMUM POSSIBLE VALUES
 *
 *
 *****************************************************************************/
template<class T, bool isBuiltin = std::is_arithmetic<T>::value>
struct numeric_max
{
	static constexpr T value = std::numeric_limits<T>::max();
};


//-------------------------------------------------------------------
template<class T>
struct numeric_max<T,false>
{
	static constexpr T value = std::numeric_limits<numeric_t<T>>::max();
};






/*****************************************************************************
 *
 *
 * MINIMUM POSSIBLE VALUES
 *
 *
 *****************************************************************************/
template<class T, bool isBuiltin = std::is_arithmetic<T>::value>
struct numeric_min
{
	static constexpr T value = std::numeric_limits<T>::min();
};


//-------------------------------------------------------------------
template<class T>
struct numeric_min<T,false>
{
	static constexpr T value = std::numeric_limits<numeric_t<T>>::min();
};






/*****************************************************************************
 *
 *
 * LOWEST POSSIBLE VALUES
 *
 *
 *****************************************************************************/
template<class T, bool isBuiltin = std::is_arithmetic<T>::value>
struct numeric_lowest
{
	static constexpr T value = std::numeric_limits<T>::lowest();
};


//-------------------------------------------------------------------
template<class T>
struct numeric_lowest<T,false>
{
	static constexpr T value = std::numeric_limits<numeric_t<T>>::lowest();
};







/*****************************************************************************
 *
 *
 * PRECISION CONTROL
 *
 *
 *****************************************************************************/
template<class T, bool isBuiltin = std::is_arithmetic<T>::value>
struct tolerance
{
	static constexpr T value = (T(100) * std::numeric_limits<T>::epsilon());
};


//-------------------------------------------------------------------
template<class T>
struct tolerance<T,false>
{
	static constexpr T value = tolerance<numeric_t<T>>::value;
};


//-------------------------------------------------------------------
template<>
struct tolerance<float,true>
{
	static constexpr float value = 10e-5;
};


//-------------------------------------------------------------------
template<>
struct tolerance<double,true>
{
	static constexpr double value = 10e-12;
};


//-------------------------------------------------------------------
template<>
struct tolerance<long double,true>
{
	static constexpr long double value = 10e-16;
};






/*****************************************************************************
 *
 *
 * LIMITS
 *
 *
 *****************************************************************************/
template<class T>
struct numeric_limits :
	public std::numeric_limits<T>
{
	static constexpr T tolerance() {return num::tolerance<T>::value; }
};




}  // namespace num

}  // namespace am


#endif
