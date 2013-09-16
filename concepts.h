/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/

#ifndef AM_NUMERIC_CONCEPTS_H_
#define AM_NUMERIC_CONCEPTS_H_


#include <type_traits>
#include <utility>
#include <limits>
#include <complex>



namespace num {


/*****************************************************************************
 *
 *
 * FORWARD DECLARATIONS
 *
 *
 *****************************************************************************/
template<class>
class dual;

template<class>
class scomplex;

template<class>
class rational;

class integer;






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
//	-> decltype(a == a || a != a, std::true_type{});
//---------------------------------------------------------
//template<class T>
//constexpr auto
//check_is_equal_comparable(T a, long)
//	-> std::false_type;



//-------------------------------------------------------------------
//template<class T>
//constexpr auto
//check_is_less_comparable(T a, int)
//	-> decltype(a < a || a <= a, std::true_type{});
//---------------------------------------------------------
//template<class T>
//constexpr auto
//check_is_less_comparable(T a, long)
//	-> std::false_type;



//-------------------------------------------------------------------
//template<class T>
//constexpr auto
//check_is_greater_comparable(T a, int)
//	-> decltype(a > a || a >= a, std::true_type{});
//---------------------------------------------------------
//template<class T>
//constexpr auto
//check_is_greater_comparable(T a, long)
//	-> std::false_type;


}  // namespace detail




/*****************************************************************************
 *
 *
 * CONCEPT CHECKING BUILDING BLOCKS
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



//-------------------------------------------------------------------
// INTEGRAL
//-------------------------------------------------------------------
template<class T>
struct is_integral :
	public std::is_integral<T>
{};

//-----------------------------------------------------
class integer;
template<>
struct is_integral<integer> :
	public std::true_type
{};



//-------------------------------------------------------------------
// UNSIGNED INTEGRAL
//-------------------------------------------------------------------
template<class T>
struct is_unsigned :
	public std::is_unsigned<T>
{};



//-------------------------------------------------------------------
// FLOATING POINT
//-------------------------------------------------------------------
template<class T>
struct is_floating_point :
	public std::is_floating_point<T>
{};

//-----------------------------------------------------
//class decimal;
//template<>
//struct is_floating_point<decimal> :
//	public std::true_type
//{};

//-----------------------------------------------------
template<class T>
struct is_floating_point<std::complex<T>> :
	public std::integral_constant<bool, is_floating_point<T>::value>
{};

//-----------------------------------------------------
template<class T>
struct is_floating_point<dual<T>> :
	public std::integral_constant<bool, is_floating_point<T>::value>
{};

//-----------------------------------------------------
template<class T>
struct is_floating_point<scomplex<T>> :
	public std::integral_constant<bool, is_floating_point<T>::value>
{};






/*****************************************************************************
 *
 *
 * CONCEPT CHECKING TRAITS CLASSES
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// NUMBER
//-------------------------------------------------------------------
template<class H, class... T>
struct is_number :
	public std::integral_constant<bool,
		is_number<typename std::decay<H>::type>::value &&
		is_number<typename std::decay<T>::type...>::value>
{};

//-----------------------------------------------------
template<class T>
struct is_number<T> :
	public std::integral_constant<bool,
		std::is_arithmetic<typename std::decay<T>::type>::value>
{};

//-----------------------------------------------------
template<class T>
struct is_number<std::complex<T>> : public std::true_type
{};

//-----------------------------------------------------
template<class T>
struct is_number<scomplex<T>> : public std::true_type
{};

//-----------------------------------------------------
template<class T>
struct is_number<dual<T>> : public std::true_type
{};

//-----------------------------------------------------
template<class T>
struct is_number<rational<T>> : public std::true_type
{};

//-----------------------------------------------------
template<>
struct is_number<integer> :
	public std::true_type
{};


//-----------------------------------------------------
/*
template<class T>
struct is_number<T> :
	public std::integral_constant<bool, true
		&& is_addable<T>::value
		&& is_subtractable<T>::value
		&& is_multipliable<T>::value
		&& is_divisible<T>::value
		&& is_invertable<T>::value
//		&& is_equal_comparable<T>::value
		>
{};
*/


//-------------------------------------------------------------------
// INTEGRAL NUMBER
//-------------------------------------------------------------------
template<class H, class... T>
struct is_integral_number :
	public std::integral_constant<bool,
		is_integral_number<typename std::decay<H>::type>::value &&
		is_integral_number<typename std::decay<T>::type...>::value>
{};

//-----------------------------------------------------
template<class T>
struct is_integral_number<T> :
	public std::integral_constant<bool,
		is_number<typename std::decay<T>::type>::value &&
		is_integral<typename std::decay<T>::type>::value>
{};



//-------------------------------------------------------------------
// FLOTING POINT NUMBER
//-------------------------------------------------------------------
template<class H, class... T>
struct is_floating_point_number :
	public std::integral_constant<bool,
		is_floating_point_number<typename std::decay<H>::type>::value &&
		is_floating_point_number<typename std::decay<T>::type...>::value>
{};

//-----------------------------------------------------
template<class T>
struct is_floating_point_number<T> :
	public std::integral_constant<bool, true
		&& is_number<typename std::decay<T>::type>::value
		&& is_floating_point<typename std::decay<T>::type>::value>
{};


}  // namespace num




#endif
