/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/


#ifndef AM_NUM_NUMERIC_CONCEPTS_H_
#define AM_NUM_NUMERIC_CONCEPTS_H_


#include <type_traits>
#include <limits>


namespace num {


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
check_is_addable(T a, T b, long)
	-> std::false_type;



//-------------------------------------------------------------------
template<class T>
constexpr auto
check_is_subtractable(T a, T b, int)
	-> decltype(a = a - b, a -= b, b -= a, std::true_type{});
//---------------------------------------------------------
template<class T>
constexpr auto
check_is_subtractable(T a, T b, long)
	-> std::false_type;



//-------------------------------------------------------------------
template<class T>
constexpr auto
check_is_multipliable(T a, T b, int)
	-> decltype(a = a * b, a *= b, b *= a, std::true_type{});
//---------------------------------------------------------
template<class T>
constexpr auto
check_is_multipliable(T a, T b, long)
	-> std::false_type;



//-------------------------------------------------------------------
template<class T>
constexpr auto
check_is_divisible(T a, T b, int)
	-> decltype(a = a / b, a /= b, b /= a, std::true_type{});
//---------------------------------------------------------
template<class T>
constexpr auto
check_is_divisible(T a, T b, long)
	-> std::false_type;



//-------------------------------------------------------------------
template<class T>
constexpr auto
check_is_invertable(T a, int)
	-> decltype(a = -a, std::true_type{});
//---------------------------------------------------------
template<class T>
constexpr auto
check_is_invertable(T a, long)
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
//template<class T>
//struct is_equal_comparable : public
//	decltype(detail::check_is_equal_comparable(std::declval<T>(), 0))
//{};


//-------------------------------------------------------------------
//template<class T>
//struct is_less_comparable : public
//	decltype(detail::check_is_less_comparable(std::declval<T>(), 0))
//{};


//-------------------------------------------------------------------
//template<class T>
//struct is_greater_comparable : public
//	decltype(detail::check_is_greater_comparable(std::declval<T>(), 0))
//{};






/*****************************************************************************
 *
 *
 * CONCEPT CHECKING TRAITS CLASSES
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class H, class... T>
struct is_number :
	public std::integral_constant<bool,
		is_number<H>::value && is_number<T...>::value>
{};

//-----------------------------------------------------
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



}  // namespace num



#endif
