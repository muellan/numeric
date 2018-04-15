/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2017 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_LIMITS_H_
#define AM_NUMERIC_LIMITS_H_


#include <limits>
#include <cmath>

#include "traits.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *****************************************************************************/
template<class T>
constexpr T highest = std::numeric_limits<numeric_t<T>>::max();

template<class T>
constexpr T lowest  = std::numeric_limits<numeric_t<T>>::lowest();

template<class T>
constexpr T min_value = std::numeric_limits<numeric_t<T>>::min();

template<class T>
constexpr T max_value = std::numeric_limits<numeric_t<T>>::max();


template<class T>
constexpr T epsilon = std::numeric_limits<T>::epsilon();



/*****************************************************************************
 *
 *****************************************************************************/
namespace detail {

template<class T, bool isBuiltin = std::is_arithmetic<T>::value>
struct tolerance {
    static constexpr T
    value() noexcept {return (T(100) * std::numeric_limits<T>::epsilon()); }
};

template<class T>
struct tolerance<T,false> {
    static constexpr T
    value() noexcept {return tolerance<numeric_t<T>>::value(); }
};

template<>
struct tolerance<float,true> {
    static constexpr float
    value() noexcept {return 10e-5f; }
};

template<>
struct tolerance<double,true> {
    static constexpr double
    value() noexcept {return 10e-12; }
};

template<>
struct tolerance<long double,true> {
    static constexpr long double
    value() noexcept {return static_cast<long double>(10e-16); }
};

} // namespace detail

template<class T>
constexpr T tolerance = detail::tolerance<T>::value();



/*****************************************************************************
 *
 *
 *****************************************************************************/
template<class T>
inline int 
decimal_places() noexcept {
    using std::log10;
    return static_cast<int>(log10(1 / static_cast<long double>(tolerance<T>())));
}



/*****************************************************************************
 *
 *
 *****************************************************************************/
namespace detail {

template<class T>
struct infinity
{
    static constexpr T
    value() noexcept {
        return std::numeric_limits<T>::has_infinity
                   ? std::numeric_limits<T>::infinity()
                   : std::numeric_limits<T>::max();
    }
};

} // namespace detail


template<class T>
constexpr T infinity = detail::infinity<T>::value();


}  // namespace num
}  // namespace am


#endif
