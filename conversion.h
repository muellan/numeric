/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 *2008-2014  André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_CONVERSION_H_
#define AM_NUMERIC_CONVERSION_H_


#include "traits.h"


namespace am {
namespace num {


/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_floating_point<decay_t<T>>::value>::type>
inline constexpr floating_point_t<decay_t<T>>
make_real(T&& x)
{
    return floating_point_t<decay_t<T>>(std::forward<T>(x));
}

//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    is_floating_point<decay_t<T>>::value>::type>
inline constexpr auto
make_real(T&& x) -> decltype(x)
{
    return std::forward<T>(x);
}




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_unsigned<decay_t<T>>::value>::type>
inline constexpr unsigned_t<decay_t<T>>
make_unsigned(T&& x)
{
    return unsigned_t<decay_t<T>>(std::forward<T>(x));
}

//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    is_unsigned<decay_t<T>>::value>::type>
inline constexpr auto
make_unsigned(T&& x) -> decltype(x)
{
    return std::forward<T>(x);
}




/*****************************************************************************
 *
 *
 *
 *****************************************************************************/
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    is_unsigned<decay_t<T>>::value>::type>
inline constexpr signed_t<decay_t<T>>
make_signed(T&& x)
{
    return signed_t<decay_t<T>>(std::forward<T>(x));
}

//-------------------------------------------------------------------
template<class T, class = typename std::enable_if<
    is_number<decay_t<T>>::value &&
    !is_unsigned<decay_t<T>>::value>::type>
inline constexpr auto
make_signed(T&& x) -> decltype(x)
{
    return std::forward<T>(x);
}



}  // namespace num
}  // namespace am


#endif
