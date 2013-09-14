#ifndef AM_NUMERIC_RATIONAL_H_
#define AM_NUMERIC_RATIONAL_H_

#include <cmath>
#include <cfloat>

#include "concepts.h"
#include "constants.h"
#include "narrowing.h"


namespace num {


/*****************************************************************************
 *
 * @brief rational number class
 *
 * @tparam IntT type of numerator and denominator
 *
 *
 * TODO include auto-normalizing mechanism (enabled by 2nd template parameter)
 *
 *****************************************************************************/
template<class IntT>
class rational
{
	static_assert(is_integral_number<IntT>::value,
		"rational<T>: T must be an integral number");

public:
	//---------------------------------------------------------------
	// TYPES
	//---------------------------------------------------------------
	using value_type      = IntT;
	using const_reference = const value_type&;


	//---------------------------------------------------------------
	// CONSTRUCTION / DESTRUCTION
	//---------------------------------------------------------------

	//-----------------------------------------------------
	/// @brief
	constexpr
	rational() = default;

	//-----------------------------------------------------
	/// @brief
	constexpr
	rational(const value_type& wholes):
		n_{wholes}, d_{value_type(1)}
	{}

	//-----------------------------------------------------
	/// @brief
	template<class T, class = typename
		std::enable_if<is_integral_number<T>::value>::type>
	explicit constexpr
	rational(const T& wholes):
		n_{wholes}, d_{value_type(1)}
	{
		AM_CHECK_NARROWING(value_type,T)
	}


	//-----------------------------------------------------
	/// @brief
	constexpr
	rational(const value_type& numerator, const value_type& denominator):
		n_{numerator}, d_{denominator}
	{}

	//-----------------------------------------------------
	/// @brief
	template<class T1, class T2, class = typename
		std::enable_if<is_integral_number<T1,T2>::value>::type>
	constexpr
	rational(const T1& numerator, const T2& denominator):
		n_{value_type(numerator)}, d_{value_type(denominator)}
	{
		AM_CHECK_NARROWING2(value_type,T1,T2)
	}

	//-----------------------------------------------------
	constexpr
	rational(const rational&) = default;


	//---------------------------------------------------------------
	// ASSIGNMENT
	//---------------------------------------------------------------
	rational&
	operator = (const rational&) = default;

	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_integral_number<T>::value>::type>
	rational&
	operator = (const T& n)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_ = n;
		d_ = value_type(0);
		return *this;
	}

	//-----------------------------------------------------
	/// @brief
	template<class T1, class T2, class = typename
		std::enable_if<is_integral_number<T1,T2>::value>::type>
	rational&
	assign(const T1& numerator, const T2& denominator)
	{
		AM_CHECK_NARROWING2(value_type,T1,T2)

		n_ = numerator;
		d_ = denominator;
		return *this;
	}


	//---------------------------------------------------------------
	// ACCESS
	//---------------------------------------------------------------
	constexpr const_reference
	numer() const noexcept {
		return n_;
	}
	//-----------------------------------------------------
	constexpr const_reference
	denom() const noexcept {
		return d_;
	}

	//---------------------------------------------------------------
	template<class T>
	explicit operator
	T() const {
		return (T(n_) / T(d_));
	}


	//---------------------------------------------------------------
	rational&
	invert()
	{
		using std::swap;

		swap(n_, d_);
		if(d_ < value_type(0)) {
			n_ = -n_;
			d_ = -d_;
		}
		return *this;
	}

	//---------------------------------------------------------
	rational&
	negate() noexcept {
		n_ = -n_;
		return *this;
	}


	//---------------------------------------------------------------
	// rational (op)= value_type
	//---------------------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_integral_number<T>::value>::type>
	rational&
	operator += (const T& v)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_ += (v * d_);
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_integral_number<T>::value>::type>
	rational&
	operator -= (const T& v)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_ -= (v * d_);
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_integral_number<T>::value>::type>
	rational&
	operator *= (const T& v)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_ *= v;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_integral_number<T>::value>::type>
	rational&
	operator /= (const T& v)
	{
		AM_CHECK_NARROWING(value_type,T)

		d_ *= v;
		return *this;
	}


	//---------------------------------------------------------------
	// increment / decrement
	//---------------------------------------------------------------
	rational&
	operator ++ () {
		n_ += d_;
		return *this;
	}
	//-----------------------------------------------------
	rational&
	operator -- () {
		n_ -= d_;
		return *this;
	}

	//-----------------------------------------------------
	rational
	operator ++ (int) {
		auto old = *this;
		++*this;
		return old;
	}
	//-----------------------------------------------------
	rational
	operator -- (int) {
		auto old = *this;
		--*this;
		return old;
	}


	//---------------------------------------------------------------
	// rational (op)= rational with different value_type
	//---------------------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_integral_number<T>::type>::type>
	rational&
	operator += (const rational<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_ = n_ * o.denom() + o.numer() * d_;
		d_ *= o.denom();
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_integral_number<T>::type>::type>
	rational&
	operator -= (const rational<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_ = n_ * o.denom() - o.numer() * d_;
		d_ *= o.denom();
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_integral_number<T>::type>::type>
	rational&
	operator *= (const rational<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_ *= o.numer();
		d_ *= o.denom();
		normalize();
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_integral_number<T>::type>::type>
	rational&
	operator /= (const rational<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_ *= o.denom();
		d_ *= o.numer();
		normalize();
		return *this;
	}


	//---------------------------------------------------------------
	void
	normalize()
	{
		using std::abs;

		auto x = gcd(abs(n_), d_);
		if(x > value_type(1)) {
			n_ /= x;
			d_ /= x;
		}
	}


private:
	//---------------------------------------------------------------
	static value_type
	gcd(value_type a, value_type b)
	{
		while(b > value_type(0)) {
			auto m = value_type(a % b);
			a = b;
			b = m;
		}
		return a;
	}

	//---------------------------------------------------------------
	value_type n_;
	value_type d_;

};








/*****************************************************************************
 *
 *
 *
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<
		is_integral_number<T1>::value &&
		is_integral_number<T2>::value,T1>::type>
inline constexpr
rational<typename std::common_type<T1,T2>::type>
make_rational(const T1& a, const T2& b)
{
	return rational<typename std::common_type<T1,T2>::type>{a,b};
}


//-------------------------------------------------------------------
template<class T>
inline constexpr
rational<T>
make_rational(const T& x)
{
	return rational<T>{x};
}



//-------------------------------------------------------------------
// I/O
//-------------------------------------------------------------------
template<class Ostream, class T>
inline Ostream&
operator << (Ostream& os, const rational<T>& x)
{
	return (os << x.numer() << " " << x.denom() );
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const rational<T>& d)
{
	return (os << d.numer() << "/" << d.denom());
}






/*****************************************************************************
 *
 *
 * ACCESS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
const T&
numer(const rational<T>& x) {
	return x.numer();
}


//-------------------------------------------------------------------
template<class T>
const T&
denom(const rational<T>& x) {
	return x.denom();
}


//-------------------------------------------------------------------
/// @brief whole parts
template<class T>
T
wholes(const rational<T>& x)
{
	using std::floor;
	return static_cast<T>(floor(static_cast<real_t>(x)));
}


//-------------------------------------------------------------------
/// @brief
template<class T>
inline rational<T>
normalized(rational<T> x)
{
	x.normalize();
	return x;
}


//-------------------------------------------------------------------
/// @brief
template<class T>
inline constexpr
rational<T>
reciprocal(const rational<T>& x)
{
	return (x.numer() < T(0))
		? rational<T>{-x.denom(), -x.numer()}
		: rational<T>{x.denom(), x.numer()};
}






/*****************************************************************************
 *
 *
 * COMPARISON
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T1, class T2>
inline bool
operator == (const rational<T1>& x, const rational<T2>& y)
{
	return ((x.numer() * y.denom()) == (y.numer() * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
operator != (const rational<T1>& x, const rational<T2>& y)
{
	return ((x.numer() * y.denom()) != (y.numer() * x.denom()));
}


//-------------------------------------------------------------------
template<class T1, class T2, class T3 = typename std::common_type<T1,T2>::type>
inline constexpr bool
approx_equal(const rational<T1>& a, const rational<T2>& b,
	const T3& tolerance = epsilon<T3>::value)
{
	return approx_equal(a.numer() * b.denom(), b.numer() * a.denom(), tolerance);
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_1(const rational<T>& x, const T& tolerance = epsilon<T>::value)
{
	return approx_1(real_t(x.numer()) / real_t(x.denom()), tolerance);
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_0(const rational<T>& x, const T& tolerance = epsilon<T>::value)
{
	return approx_0(x.numer(), tolerance);
}



//-------------------------------------------------------------------
// COMPARISON
//-------------------------------------------------------------------
template<class T1, class T2>
inline bool
operator > (const rational<T1>& x, const rational<T2>& y)
{
	return ((x.numer() * y.denom()) > (y.numer() * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
operator >= (const rational<T1>& x, const rational<T2>& y)
{
	return ((x.numer() * y.denom()) >= (y.numer() * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
operator < (const rational<T1>& x, const rational<T2>& y)
{
	return ((x.numer() * y.denom()) < (y.numer() * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
operator <= (const rational<T1>& x, const rational<T2>& y)
{
	return ((x.numer() * y.denom()) <= (y.numer() * x.denom()));
}



//-------------------------------------------------------------------
// COMPARISON WITH PLAIN NUMBERS
//-------------------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_integral_number<T2>::value>::type>
inline bool
operator > (const rational<T1>& x, const T2& r)
{
	return (x.numer() > (r * x.denom()));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_integral_number<T2>::value>::type>
inline bool
operator > (const T2& r, const rational<T1>& x)
{
	return (x.numer() > (r * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_integral_number<T2>::value>::type>
inline bool
operator >= (const rational<T1>& x, const T2& r)
{
	return (x.numer() >= (r * x.denom()));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_integral_number<T2>::value>::type>
inline bool
operator >= (const T2& r, const rational<T1>& x)
{
	return (x.numer() >= (r * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_integral_number<T2>::value>::type>
inline bool
operator < (const rational<T1>& x, const T2& r)
{
	return (x.numer() < (r * x.denom()));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_integral_number<T2>::value>::type>
inline bool
operator < (const T2& r, const rational<T1>& x)
{
	return (x.numer() < (r * x.denom()));
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_integral_number<T2>::value>::type>
inline bool
operator <= (const rational<T1>& x, const T2& r)
{
	return (x.numer() <= (r * x.denom()));
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_integral_number<T2>::value>::type>
inline bool
operator <= (const T2& r, const rational<T1>& x)
{
	return (x.numer() <= (r * x.denom()));
}






/*****************************************************************************
 *
 *
 * ARITHMETIC
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// ADDITION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
rational<typename std::common_type<T1,T2>::type>
operator + (const rational<T1> x, const rational<T2>& y)
{
	return rational<typename std::common_type<T1,T2>::type>{
		(x.numer() * y.denom()) + (y.numer() * x.denom()),
		x.denom() * y.denom() };
}

//---------------------------------------------------------
template<class T>
inline constexpr
rational<T>
operator + (const rational<T> x, const T& y)
{
	return rational<T>{x.numer() + (y * x.denom()), x.denom()};
}



//-------------------------------------------------------------------
// SUBTRACTION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
rational<typename std::common_type<T1,T2>::type>
operator - (const rational<T1> x, const rational<T2>& y)
{
	return rational<typename std::common_type<T1,T2>::type>{
		(x.numer() * y.denom()) - (y.numer() * x.denom()),
		x.denom() * y.denom() };
}

//---------------------------------------------------------
template<class T>
inline constexpr
rational<T>
operator - (const rational<T> x, const T& y)
{
	return rational<T>{x.numer() - (y * x.denom()), x.denom()};
}



//-------------------------------------------------------------------
// MULTIPLIPCATION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
rational<typename std::common_type<T1,T2>::type>
operator * (const rational<T1> x, const rational<T2>& y)
{
	return rational<typename std::common_type<T1,T2>::type>{
		x.numer() * y.numer(), x.denom() * y.denom() };
}

//---------------------------------------------------------
template<class T>
inline constexpr
rational<T>
operator * (const rational<T> x, const T& y)
{
	return rational<T>{x.numer() * y, x.denom()};
}



//-------------------------------------------------------------------
// DIVISION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
rational<typename std::common_type<T1,T2>::type>
operator / (const rational<T1> x, const rational<T2>& y)
{
	return rational<typename std::common_type<T1,T2>::type>{
		x.numer() * y.denom(), x.denom() * y.numer() };
}

//---------------------------------------------------------
template<class T>
inline constexpr
rational<T>
operator / (const rational<T> x, const T& y)
{
	return rational<T>{x.numer(), x.denom() * y};
}



//-------------------------------------------------------------------
// EXPONENTIATION
//-------------------------------------------------------------------
template<class T>
inline
rational<T>
operator ^ (const rational<T> b, const T& e)
{
	return pow(b,e);
}



//-------------------------------------------------------------------
// ADDITIVE INVERSION
//-------------------------------------------------------------------
template<class T>
inline constexpr
rational<T>
operator - (const rational<T> x)
{
	return rational<T>{-x.numer(), x.denom()};
}






/*****************************************************************************
 *
 *
 * FUNCTIONS
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
template<class T>
inline
rational<T>
ceil(const rational<T>& x)
{
	using std::ceil;
	return rational<T>{ceil(static_cast<T>(x))};
}

//---------------------------------------------------------
template<class T>
inline
rational<T>
floor(const rational<T>& x)
{
	return rational<T>{wholes(x)};
}



//-------------------------------------------------------------------
// ABSOLUTE
//-------------------------------------------------------------------
/// @brief absolute value
template<class T>
inline rational<T>
abs(const rational<T>& x)
{
	using std::abs;
	return rational<T>{abs(x.numer()), abs(x.denom())};
}



//-------------------------------------------------------------------
// EXPONENTIATION
//-------------------------------------------------------------------
template<class T>
inline
rational<T>
pow(const rational<T> b, const T& e)
{
	using std::pow;

	return ((e > T(0))
		? rational<T>{pow(b.numer(),e), pow(b.denom(),e)}
		: rational<T>{pow(b.denom(),e), pow(b.numer(),e)} );
}



//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
template<class T>
inline bool
isfinite(const rational<T>& x)
{
	using std::isfinite;
	return (isfinite(x.numer()) && isfinite(x.denom()));
}

//---------------------------------------------------------
template<class T>
inline bool
isinf(const rational<T>& x)
{
	using std::isinf;
	return (isinf(x.numer()) || isinf(x.denom()));
}

//---------------------------------------------------------
template<class T>
inline bool
isnan(const rational<T>& x)
{
	using std::isnan;
	return (isnan(x.numer()) || isnan(x.denom()));
}

//---------------------------------------------------------
template<class T>
inline bool
isnormal(const rational<T>& x)
{
	using std::isnormal;
	return (isnormal(x.numer()) && isnormal(x.denom()));
}


}  // namespace num


#endif
