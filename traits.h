/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2014 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_TRAITS_H_
#define AM_NUMERIC_TRAITS_H_

#include <type_traits>
#include <utility>
#include <complex>
#include <initializer_list>
#include <array>


#include "constants.h"


namespace am {
namespace num {



/*****************************************************************************
 *
 *
 *
 *****************************************************************************/

template<class T>
using decay_t = typename std::decay<T>::type;






/*****************************************************************************
 *
 *
 * CONCEPT CHECKING LOW-LEVEL TESTS
 *
 *
 *****************************************************************************/

namespace detail {


//-------------------------------------------------------------------
template<class T>
constexpr auto
check_is_addable(T a, T b, int)
    -> decltype(a = a + b, a += b, b += a, std::true_type{});
//---------------------------------------------------------
template<class T>
constexpr auto
check_is_addable(T, T, long)
    -> std::false_type;



//-------------------------------------------------------------------
template<class T>
constexpr auto
check_is_subtractable(T a, T b, int)
    -> decltype(a = a - b, a -= b, b -= a, std::true_type{});
//---------------------------------------------------------
template<class T>
constexpr auto
check_is_subtractable(T, T, long)
    -> std::false_type;



//-------------------------------------------------------------------
template<class F>
constexpr auto
check_is_additive(F a, F b, int)
    -> decltype(
        a = a + b, a += b, b += a, a = a - b, a -= b, b -= a,
        std::true_type{});
//---------------------------------------------------------
template<class F>
constexpr auto
check_is_additive(F a, F b, long)
    -> std::false_type;



//-------------------------------------------------------------------
template<class T>
constexpr auto
check_is_multipliable(T a, T b, int)
    -> decltype(a = a * b, a *= b, b *= a, std::true_type{});
//---------------------------------------------------------
template<class T>
constexpr auto
check_is_multipliable(T, T, long)
    -> std::false_type;



//-------------------------------------------------------------------
template<class T>
constexpr auto
check_is_divisible(T a, T b, int)
    -> decltype(a = a / b, a /= b, b /= a, std::true_type{});
//---------------------------------------------------------
template<class T>
constexpr auto
check_is_divisible(T, T, long)
    -> std::false_type;



//-------------------------------------------------------------------
template<class T>
constexpr auto
check_is_invertable(T a, int)
    -> decltype(a = -a, std::true_type{});
//---------------------------------------------------------
template<class T>
constexpr auto
check_is_invertable(T, long)
    -> std::false_type;



//-------------------------------------------------------------------
//template<class T>
//constexpr auto
//check_is_equal_comparable(T a, int)
//    -> decltype(a == a || a != a, std::true_type{});
//---------------------------------------------------------
//template<class T>
//constexpr auto
//check_is_equal_comparable(T a, long)
//    -> std::false_type;



//-------------------------------------------------------------------
//template<class T>
//constexpr auto
//check_is_less_comparable(T a, int)
//    -> decltype(a < a || a <= a, std::true_type{});
//---------------------------------------------------------
//template<class T>
//constexpr auto
//check_is_less_comparable(T a, long)
//    -> std::false_type;



//-------------------------------------------------------------------
//template<class T>
//constexpr auto
//check_is_greater_comparable(T a, int)
//    -> decltype(a > a || a >= a, std::true_type{});
//---------------------------------------------------------
//template<class T>
//constexpr auto
//check_is_greater_comparable(T a, long)
//    -> std::false_type;



//-------------------------------------------------------------------
template<class T>
constexpr auto
check_has_dimensions(T&& t, int)
    -> decltype(t.dimensions(), std::true_type{});

template<class T>
constexpr auto
check_has_dimensions(T&&, long) -> std::false_type;



//-------------------------------------------------------------------
template<class T>
constexpr auto
check_has_min(T&& t, int)
    -> decltype(min(t), std::true_type{});

template<class T>
constexpr auto
check_has_min(T&&, long) -> std::false_type;



//-------------------------------------------------------------------
template<class T>
constexpr auto
check_has_max(T&& t, int)
    -> decltype(max(t), std::true_type{});

template<class T>
constexpr auto
check_has_max(T&&, long) -> std::false_type;



//-------------------------------------------------------------------
template<class T>
constexpr auto
check_has_numeric_type(int)
    -> decltype(
        std::declval<typename T::numeric_type>(),
        std::true_type{});
//---------------------------------------------------------
template<class>
constexpr auto
check_has_numeric_type(long)
    -> std::false_type;


}  // namespace detail






/*****************************************************************************
 *
 * @brief underlying numeric type
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
struct has_numeric_type: public
    decltype(detail::check_has_numeric_type<T>(0))
{};



//-------------------------------------------------------------------
template<class T, bool isBuiltin = std::is_arithmetic<T>::value>
struct numeric_type
{
    using type = typename T::numeric_type;
};

//---------------------------------------------------------
template<class T>
struct numeric_type<T,true>
{
    using type = T;
};


//---------------------------------------------------------
template<class T>
struct numeric_type<std::initializer_list<T>,false>
{
    using type = typename numeric_type<T>::type;
};

//---------------------------------------------------------
template<class T, std::size_t n>
struct numeric_type<std::array<T,n>,false>
{
    using type = typename numeric_type<T>::type;
};

//---------------------------------------------------------
template<class T>
struct numeric_type<std::complex<T>,false>
{
    using type = typename numeric_type<T>::type;
};

//---------------------------------------------------------
template<class T>
using numeric_t = typename numeric_type<T>::type;






/*****************************************************************************
 *
 * @brief common numeric type
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T, class... Ts>
struct common_numeric_type
{
    using type =
        typename std::common_type<
            numeric_t<decay_t<T>>,
            typename common_numeric_type<Ts...>::type>::type;
};

//---------------------------------------------------------
template<class T>
struct common_numeric_type<T>
{
    using type = numeric_t<decay_t<T>>;
};

//---------------------------------------------------------
template<class... Ts>
using common_numeric_t = typename common_numeric_type<Ts...>::type;



//-------------------------------------------------------------------
template<class... Ts>
using floating_point_t = common_numeric_t<real_t,Ts...>;



//-------------------------------------------------------------------
template<class T, class T2>
struct common_numeric_type<std::complex<T>,T2>
{
    using type = std::complex<common_numeric_t<T,T2>>;
};
//---------------------------------------------------------
template<class T, class T2>
struct common_numeric_type<T2,std::complex<T>>
{
    using type = common_numeric_t<std::complex<T>,T2>;
};






/*****************************************************************************
 *
 * @brief
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T, class... Ts>
struct unsigned_type
{
    using type = typename unsigned_type<common_numeric_t<T,Ts...>>::type;
};

//---------------------------------------------------------
template<class T>
struct unsigned_type<T> {using type = decay_t<T>; };

template<>
struct unsigned_type<signed char>  {using type = unsigned char; };

template<>
struct unsigned_type<signed short> {using type = unsigned short; };

template<>
struct unsigned_type<signed int>   {using type = unsigned int; };

template<>
struct unsigned_type<signed long>  {using type = unsigned long; };

template<>
struct unsigned_type<signed long long> {using type = unsigned long long; };


template<class T>
struct unsigned_type<std::complex<T>> {
    using type = std::complex<typename unsigned_type<T>::type>;
};


//-------------------------------------------------------------------
template<class... Ts>
using unsigned_t = typename unsigned_type<decay_t<Ts>...>::type;






/*****************************************************************************
 *
 * @brief
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T, class... Ts>
struct signed_type
{
    using type = typename signed_type<common_numeric_t<T,Ts...>>::type;
};

//---------------------------------------------------------
template<class T>
struct signed_type<T> {using type = T; };

template<>
struct signed_type<unsigned char>  {using type = signed char; };

template<>
struct signed_type<unsigned short> {using type = signed short; };

template<>
struct signed_type<unsigned int>   {using type = signed int; };

template<>
struct signed_type<unsigned long>  {using type = signed long; };

template<>
struct signed_type<unsigned long long> {using type = signed long long; };


template<class T>
struct signed_type<std::complex<T>> {
    using type = std::complex<typename signed_type<T>::type>;
};


//-------------------------------------------------------------------
template<class... Ts>
using signed_t = typename signed_type<decay_t<Ts>...>::type;






/*****************************************************************************
 *
 * @brief dimensions
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
struct has_dimensions: public
    decltype(detail::check_has_dimensions(std::declval<T>(), 0))
{};



//-------------------------------------------------------------------
template<class T, bool hasdim = has_dimensions<T>::value>
struct dimensions :
    std::integral_constant<dims_t, dims_t(0)>
{};

//---------------------------------------------------------
template<class T>
struct dimensions<T,true> :
    std::integral_constant<
        typename std::decay<decltype(T::dimensions())>::type,
        T::dimensions()>
{};

//-------------------------------------------------------------------
template<class T, std::size_t n>
struct dimensions<std::array<T,n>,false> :
    std::integral_constant<dims_t,n>
{};



//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
template<dims_t n, class H, class... T>
struct same_dimension :
    std::integral_constant<bool,
        (same_dimension<n,H>::value &&
         same_dimension<n,T...>::value)>
{
};

//---------------------------------------------------------
template<dims_t n, class T>
struct same_dimension<n,T> :
    std::integral_constant<bool, (dimensions<T>::value == n)>
{};



//-------------------------------------------------------------------
template<class T>
struct dimension_type
{
    using type = typename std::decay<decltype(dimensions<T>::value)>::type;
};

template<class T>
using dimension_t = typename dimension_type<T>::type;



//-------------------------------------------------------------------
template<class T>
struct component
{
    using type =
        typename std::decay<decltype(std::declval<T>()[0])>::type;
};

template<class T>
using component_t = typename component<T>::type;






/*****************************************************************************
 *
 *
 * SPECIAL FUNCTION OVERLOADS FOR BUILTIN-TYPES
 *
 *
 *****************************************************************************/


//---------------------------------------------------------
template<class T, class = typename std::enable_if<
    std::is_arithmetic<typename std::decay<T>::type>::value>::type>
inline constexpr auto
real(T&& x) -> decltype(std::forward<T>(x))
{
    return std::forward<T>(x);
}


//---------------------------------------------------------
template<class T, class = typename std::enable_if<
    std::is_arithmetic<typename std::decay<T>::type>::value>::type>
inline constexpr T
imag(const T&)
{
    return T(0);
}






/*****************************************************************************
 *
 *
 * ARITHMETIC TRAITS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
struct is_addable : public
    decltype(detail::check_is_addable(std::declval<T>(), std::declval<T>(), 0))
{};


//-------------------------------------------------------------------
template<class T>
struct is_subtractable : public
    decltype(detail::check_is_subtractable(std::declval<T>(), std::declval<T>(), 0))
{};


//-------------------------------------------------------------------
template<class F>
struct is_additive : public
    decltype(detail::check_is_additive(std::declval<F>(), std::declval<F>(), 0))
{};


//-------------------------------------------------------------------
template<class T>
struct is_multipliable : public
    decltype(detail::check_is_multipliable(std::declval<T>(), std::declval<T>(), 0))
{};


//-------------------------------------------------------------------
template<class T>
struct is_divisible : public
    decltype(detail::check_is_divisible(std::declval<T>(), std::declval<T>(), 0))
{};


//-------------------------------------------------------------------
template<class T>
struct is_invertable : public
    decltype(detail::check_is_invertable(std::declval<T>(), 0))
{};






/*****************************************************************************
 *
 *
 * TYPE DOMAIN TRAITS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// INTEGRAL
//-------------------------------------------------------------------
template<class H, class... T>
struct is_integral :
    std::integral_constant<bool,
        is_integral<decay_t<H>>::value &&
        is_integral<decay_t<T>...>::value>
{};

//-----------------------------------------------------
template<class T>
struct is_integral<T> :
    std::integral_constant<bool,
        std::is_integral<decay_t<T>>::value>
{};



//-------------------------------------------------------------------
// UNSIGNED INTEGRAL
//-------------------------------------------------------------------
template<class T>
struct is_unsigned :
    public std::is_unsigned<decay_t<T>>
{};



//-------------------------------------------------------------------
// FLOATING POINT
//-------------------------------------------------------------------
template<class H, class... T>
struct is_floating_point :
    std::integral_constant<bool,
        is_floating_point<decay_t<H>>::value &&
        is_floating_point<decay_t<T>...>::value>
{};

//-----------------------------------------------------
template<class T>
struct is_floating_point<T> :
    public std::is_floating_point<decay_t<T>>
{};

//-----------------------------------------------------
//class decimal;
//template<>
//struct is_floating_point<decimal> :
//    std::true_type
//{};

//-----------------------------------------------------
template<class T>
struct is_floating_point<std::complex<T>> :
    std::integral_constant<bool, is_floating_point<T>::value>
{};







/*****************************************************************************
 *
 *
 * NUMBER CONCEPT
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// NUMBER
//-------------------------------------------------------------------
template<class H, class... T>
struct is_number :
    std::integral_constant<bool,
        is_number<decay_t<H>>::value &&
        is_number<decay_t<T>...>::value>
{};

//-----------------------------------------------------
template<class T>
struct is_number<T> :
    std::integral_constant<bool,
        std::is_arithmetic<decay_t<T>>::value>
{};

//-----------------------------------------------------
template<class T>
struct is_number<std::complex<T>> : std::true_type {};

template<class T>
struct is_number<std::complex<T>&> : std::true_type {};

template<class T>
struct is_number<std::complex<T>&&> : std::true_type {};

template<class T>
struct is_number<const std::complex<T>&> : std::true_type {};

template<class T>
struct is_number<const std::complex<T>> : std::true_type {};


//-----------------------------------------------------
/*
template<class T>
struct is_number<T> :
    std::integral_constant<bool, true
        && is_addable<T>::value
        && is_subtractable<T>::value
        && is_multipliable<T>::value
        && is_divisible<T>::value
        && is_invertable<T>::value
//        && is_equal_comparable<T>::value
        >
{};
*/






/*****************************************************************************
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
struct has_min: public
    decltype(detail::check_has_min(std::declval<T>(), 0))
{};

//-------------------------------------------------------------------
template<class T>
struct has_max: public
    decltype(detail::check_has_max(std::declval<T>(), 0))
{};


//-------------------------------------------------------------------
template<class T, class... R>
struct is_interval :
    std::integral_constant<bool,
        is_interval<T>::value && is_interval<R...>::value>
{};

//---------------------------------------------------------
template<class T>
struct is_interval<T> :
    std::integral_constant<bool,
        has_min<T>::value &&
        has_max<T>::value>
{};


}  // namespace num
}  // namespace am

#endif
