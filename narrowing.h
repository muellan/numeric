/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/


#ifndef AM_NUM_NUMERIC_NARROWING_H_
#define AM_NUM_NUMERIC_NARROWING_H_

#include <complex>

#include "concepts.h"



namespace num {


/*****************************************************************************
 *
 * @brief traits class for testing if a conversion is narrowing
 *
 * @details
 * non-narrowing if 'To' is the same as common type of 'F' through 'Fs'
 * and all types are either all signed or all unsigned
 *
 *
 *****************************************************************************/

namespace detail {


//-------------------------------------------------------------------
template<bool numbers, class To, class F, class... Fs>
struct is_non_narrowing_helper :
	public std::false_type
{};

//---------------------------------------------------------
template<class To, class F, class... Fs>
struct is_non_narrowing_helper<true,To,F,Fs...> :
	public std::integral_constant<bool,
		is_non_narrowing_helper<true,To,F>::value &&
		is_non_narrowing_helper<true,To,Fs...>::value>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true,To,From> :
	public std::integral_constant<bool,
		std::is_same<
			typename std::decay<To>::type,
			typename std::common_type<
				typename std::decay<From>::type,
				typename std::decay<To>::type
			>::type
		>::value &&
		( ( std::is_signed<typename std::decay<From>::type>::value &&
			std::is_signed<typename std::decay<To>::type>::value
		  ) ||
		  ( std::is_unsigned<typename std::decay<From>::type>::value &&
			std::is_unsigned<typename std::decay<To>::type>::value
		  )
		)
	>
{};



//-------------------------------------------------------------------
///TODO get rid of is_non_narrowing_helper specializations for dual, scomplex, std::complex
template<class>
class dual;

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, dual<To>, From> :
	public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, To, dual<From>> :
	public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, dual<To>, dual<From> > :
	public is_non_narrowing_helper<true,To,From>
{};



//-------------------------------------------------------------------
template<class>
class scomplex;

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, scomplex<To>, From> :
	public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, To, scomplex<From>> :
	public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, scomplex<To>, scomplex<From> > :
	public is_non_narrowing_helper<true,To,From>
{};



//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, std::complex<To>, From> :
	public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, To, std::complex<From> > :
	public is_non_narrowing_helper<true,To,From>
{};

//---------------------------------------------------------
template<class To, class From>
struct is_non_narrowing_helper<true, std::complex<To>, std::complex<From> > :
	public is_non_narrowing_helper<true,To,From>
{};



}  // namespace detail


//-------------------------------------------------------------------
template<class To, class From, class... Froms>
struct is_non_narrowing :
	public std::integral_constant<bool,
		detail::is_non_narrowing_helper<
			is_number<To,From,Froms...>::value,
			To, From,Froms...>::value
	>
{};


}  // namespace num



#endif
