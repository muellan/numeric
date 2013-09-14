#ifndef AM_NUMERIC_SPLIT_COMPLEX_H_
#define AM_NUMERIC_SPLIT_COMPLEX_H_

#include <cmath>
#include <cfloat>

#include "concepts.h"
#include "constants.h"
#include "narrowing.h"



namespace num {


/*****************************************************************************
 *
 * @brief
 * represents a split complex number (aka real tessarine) r + j * d where
 * r is the real part,
 * d is the imaginary part and
 * j is the split complex imaginary unit with j*j = +1
 *
 *****************************************************************************/
template<class NumberType>
class scomplex
{
public:

	static_assert(is_number<NumberType>::value,
		"scomplex<T>: T must be a number");


	//---------------------------------------------------------------
	// TYPES
	//---------------------------------------------------------------
	using value_type      = NumberType;
	using const_reference = const value_type&;


	//---------------------------------------------------------------
	// CONSTRUCTION / DESTRUCTION
	//---------------------------------------------------------------

	//-----------------------------------------------------
	/// @brief default constructor
	constexpr
	scomplex() = default;

	//-----------------------------------------------------
	/// @brief
	explicit constexpr
	scomplex(const value_type& a):
		n_{a,value_type(0)}
	{}

	//-----------------------------------------------------
	/// @brief
	template<class T, class = typename
		std::enable_if<is_number<T>::value>::type>
	explicit constexpr
	scomplex(const T& a):
		n_{value_type(a),value_type(0)}
	{
		AM_CHECK_NARROWING(value_type,T)
	}

	//-----------------------------------------------------
	/// @brief
	constexpr
	scomplex(const value_type& a, const value_type& b):
		n_{a, b}
	{}

	//-----------------------------------------------------
	/// @brief
	template<class T1, class T2, class = typename
		std::enable_if<is_number<T1,T2>::value>::type>
	constexpr
	scomplex(const T1& a, const T2& b):
		n_{value_type(a), value_type(b)}
	{
		AM_CHECK_NARROWING2(value_type,T1,T2)
	}

	//-----------------------------------------------------
	constexpr
	scomplex(const scomplex&) = default;


	//---------------------------------------------------------------
	// ASSIGNMENT
	//---------------------------------------------------------------
	scomplex&
	operator = (const scomplex&) = default;

	//-----------------------------------------------------
	template<class T>
	scomplex&
	operator = (const scomplex<T>& d)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] = d.n_[0];
		n_[1] = d.n_[1];
		return *this;
	}

	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_number<T>::value>::type>
	scomplex&
	operator = (const T& n)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] = n;
		n_[1] = value_type(0);
		return *this;
	}


	//-----------------------------------------------------
	/// @brief
	template<class T1, class T2, class = typename std::enable_if<
		is_number<T1,T2>::value>::type>
	scomplex&
	assign(const T1& r, const T2& d)
	{
		AM_CHECK_NARROWING2(value_type,T1,T2)

		n_[0] = r;
		n_[1] = d;
		return *this;
	}


	//---------------------------------------------------------------
	// ELEMENT ACCESS
	//---------------------------------------------------------------
	constexpr const_reference
	operator [] (std::size_t index) const noexcept
	{
		return n_[index];
	}

	//-----------------------------------------------------
	constexpr const_reference
	real() const noexcept
	{
		return n_[0];
	}

	//-----------------------------------------------------
	constexpr const_reference
	imag() const noexcept
	{
		return n_[1];
	}


	//---------------------------------------------------------------
	scomplex&
	conjugate()
	{
		n_[1] = -n_[1];
		return *this;
	}

	//---------------------------------------------------------
	scomplex&
	negate() noexcept
	{
		n_[0] = -n_[0];
		n_[1] = -n_[1];
		return *this;
	}


	//---------------------------------------------------------------
	// scomplex (op)= number
	//---------------------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_number<T>::value>::type>
	scomplex&
	operator += (const T& v)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] += v;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_number<T>::value>::type>
	scomplex&
	operator -= (const T& v)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] -= v;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_number<T>::value>::type>
	scomplex&
	operator *= (const T& v)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] *= v;
		n_[1] *= v;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_number<T>::value>::type>
	scomplex&
	operator /= (const T& v)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] /= v;
		n_[1] /= v;
		return *this;
	}


	//---------------------------------------------------------------
	// increment / decrement
	//---------------------------------------------------------------
	scomplex&
	operator ++ () {
		++n_[0];
		return *this;
	}
	//-----------------------------------------------------
	scomplex&
	operator -- () {
		--n_[0];
		return *this;
	}

	//-----------------------------------------------------
	scomplex
	operator ++ (int) {
		auto old = *this;
		++*this;
		return old;
	}
	//-----------------------------------------------------
	scomplex
	operator -- (int) {
		auto old = *this;
		--*this;
		return old;
	}


	//---------------------------------------------------------------
	// scomplex (op)= scomplex with different value_type
	//---------------------------------------------------------------
	template<class T>
	scomplex&
	operator += (const scomplex<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] += o.real();
		n_[1] += o.imag();
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	scomplex&
	operator -= (const scomplex<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] -= o.real();
		n_[1] -= o.imag();
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	scomplex&
	operator *= (const scomplex<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		auto n_0 = n_[0];
		n_[0] = (n_[0] * o.n_[0]) + (n_[1] * o.n_[1]);
		n_[1] = (n_0 * o.n_[1]) + (n_[1] * o.n_[0]);
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	scomplex&
	operator /= (const scomplex<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		auto abso_inv = value_type(1) / abs(o);
		auto n_0 = n_[0];
		n_[0] = abso_inv * ( (n_[0] * o.n_[0]) - (n_[1] * o.n_[1]) );
		n_[1] = abso_inv * ( (n_[1] * o.n_[0]) - (n_0   * o.n_[1]) );
		return *this;
	}

	//-----------------------------------------------------
	template<class T>
	scomplex&
	times_conj(const scomplex<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		auto n_0 = n_[0];
		n_[0] = (n_[0] * o.n_[0]) - (n_[1] * o.n_[1]);
		n_[1] = (n_[1] * o.n_[0]) - (n_0 * o.n_[1]);
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	scomplex&
	conj_times(const scomplex<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		auto n_0 = n_[0];
		n_[0] = (n_[0] * o.n_[0]) - (n_[1] * o.n_[1]);
		n_[1] = (n_0 * o.n_[1]) - (n_[1] * o.n_[0]);
		return *this;
	}


private:

	//---------------------------------------------------------------
	value_type n_[2];

};








/*****************************************************************************
 *
 *
 *
 *
 *
 *****************************************************************************/


//---------------------------------------------------------------
template<class T1, class T2>
inline
scomplex<typename std::common_type<T1,T2>::type>
make_scomplex(const T1& a, const T2& b)
{
	return scomplex<typename std::common_type<T1,T2>::type>{a,b};
}

//---------------------------------------------------------------
template<class T, class = typename
	std::enable_if<is_number<T>::value,T>::type>
inline
scomplex<T>
make_scomplex(const T& x)
{
	return scomplex<T>{x, T(0)};
}



//-------------------------------------------------------------------
// I/O
//-------------------------------------------------------------------
template<class Istream, class T>
inline Istream&
operator >> (Istream& is, scomplex<T>& x)
{
	return (is >> x.real() >> x.imag());
}

//---------------------------------------------------------
template<class Ostream, class T>
inline Ostream&
operator << (Ostream& os, const scomplex<T>& x)
{
	return (os << x.real() << " " << x.imag() );
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const scomplex<T>& d)
{
	return (os << "(" << d.real() << "," << d.imag() << ")" );
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
inline constexpr auto
real(const scomplex<T>& d) noexcept -> decltype(d.real())
{
	return d.real();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
imag(const scomplex<T>& d) noexcept -> decltype(d.imag())
{
	return d.imag();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr
scomplex<T>
conj(const scomplex<T>& x)
{
	return scomplex<T>{x.real(), -x.imag()};
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
operator == (const scomplex<T1>& a, const scomplex<T2>& b)
{
	return ((a.real() == b.real()) && (a.imag() == b.imag()));
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
operator != (const scomplex<T1>& a, const scomplex<T2>& b)
{
	return ((a.real() != b.real()) || (a.imag() != b.imag()));
}


//-------------------------------------------------------------------
template<class T1, class T2, class T3 = typename std::common_type<T1,T2>::type>
inline constexpr bool
approx_equal(
	const scomplex<T1>& a, const scomplex<T2>& b,
	const T3& tolerance = epsilon<T3>::value)
{
	return (approx_equal(a.real(), b.real(), tolerance) &&
		    approx_equal(a.imag(), b.imag(), tolerance) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_1(const scomplex<T>& x, const T& tolerance = epsilon<T>::value)
{
	return (
		approx_1(x.real(), tolerance) &&
		approx_0(x.imag(), tolerance) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_0(const scomplex<T>& x, const T& tolerance = epsilon<T>::value)
{
	return (
		approx_0(x.real(), tolerance) &&
		approx_0(x.imag(), tolerance) );
}



//-------------------------------------------------------------------
// COMPARISON WITH INTEGERS OR REAL NUMBERS
//-------------------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator > (const scomplex<T1>& x, const T2& r)
{
	return (x.real() > r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator > (const T2& r, const scomplex<T1>& x)
{
	return (r > x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator >= (const scomplex<T1>& x, const T2& r)
{
	return (x.real() >= r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator >= (const T2& r, const scomplex<T1>& x)
{
	return (r >= x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator < (const scomplex<T1>& x, const T2& r)
{
	return (x.real() < r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator < (const T2& r, const scomplex<T1>& x)
{
	return (r < x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator <= (const scomplex<T1>& x, const T2& r)
{
	return (x.real() <= r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator <= (const T2& r, const scomplex<T1>& x)
{
	return (r <= x.real());
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
scomplex<typename std::common_type<T1,T2>::type>
operator + (const scomplex<T1> x, const scomplex<T2>& y)
{
	return scomplex<typename std::common_type<T1,T2>::type>{
		x.real() + y.real(), x.imag() + y.imag()};
}

//---------------------------------------------------------
template<class T>
inline constexpr
scomplex<T>
operator + (const scomplex<T> x, const T& y)
{
	return scomplex<T>{x.real() + y, x.imag() + y};
}



//-------------------------------------------------------------------
// SUBTRACTION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
scomplex<typename std::common_type<T1,T2>::type>
operator - (const scomplex<T1> x, const scomplex<T2>& y)
{
	return scomplex<typename std::common_type<T1,T2>::type>{
		x.real() - y.real(), x.imag() - y.imag()};
}

//---------------------------------------------------------
template<class T>
inline constexpr
scomplex<T>
operator - (const scomplex<T> x, const T& y)
{
	return scomplex<T>{x.real() - y, x.imag() - y};
}



//-------------------------------------------------------------------
// MULTIPLIPCATION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
scomplex<typename std::common_type<T1,T2>::type>
operator * (const scomplex<T1> x, const scomplex<T2>& y)
{
	return scomplex<typename std::common_type<T1,T2>::type>{
		(x.real() * y.real()) + (x.imag() * y.imag()),
		(x.real() * y.imag()) + (x.imag() * y.real()),
	};
}

//---------------------------------------------------------
template<class T>
inline constexpr
scomplex<T>
operator * (const scomplex<T> x, const T& y)
{
	return scomplex<T>{x.real() * y, x.imag() * y};
}



//-------------------------------------------------------------------
// DIVISION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
scomplex<typename std::common_type<T1,T2>::type>
operator / (const scomplex<T1> x, const scomplex<T2>& y)
{
	using res_t = typename std::common_type<T1,T2>::type;
	auto absy = res_t(1) / abs(y);

	return scomplex<res_t>{
		absy * ( (x.real() * y.real()) - (x.imag() * y.imag()) ),
		absy * ( (x.imag() * y.real()) - (x.real() * y.imag()) )
	};
}

//---------------------------------------------------------
template<class T>
inline constexpr
scomplex<T>
operator / (const scomplex<T> x, const T& y)
{
	return scomplex<T>{x.real() / y, x.imag() / y};
}



//-------------------------------------------------------------------
// INVERSION
//-------------------------------------------------------------------
template<class T>
inline constexpr
scomplex<T>
operator - (const scomplex<T> x)
{
	return scomplex<T>{-x.real(), -x.imag()};
}



//-------------------------------------------------------------------
// SPECIAL MULTIPLICATIONS
//-------------------------------------------------------------------
template<class T1, class T2>
inline scomplex<typename std::common_type<T1,T2>::type>
times_conj(const scomplex<T1>& x, const scomplex<T2>& y)
{
	return scomplex<typename std::common_type<T1,T2>::type>{
		(x.real() * y.real()) - (x.imag() * y.imag()),
		(x.imag() * y.real()) - (x.real() * y.imag()),
	};
}

//---------------------------------------------------------
template<class T1, class T2>
inline scomplex<typename std::common_type<T1,T2>::type>
conj_times(const scomplex<T1>& x, const scomplex<T2>& y)
{
	return scomplex<typename std::common_type<T1,T2>::type>{
		(x.real() * y.real()) - (x.imag() * y.imag()),
		(x.real() * y.imag()) - (x.imag() * y.real()),
	};
}






/*****************************************************************************
 *
 *
 * FUNCTIONS
 *
 * @note f(r + e*d) = f(r) + d*f'(r) * e
 *       where e is the scomplex unit, r the real part and d the scomplex part
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
template<class T>
inline
scomplex<T>
ceil(const scomplex<T>& x)
{
	using std::ceil;
	return scomplex<T>{ceil(x.real()), ceil(x.imag())};
}

//---------------------------------------------------------
template<class T>
inline
scomplex<T>
floor(const scomplex<T>& x)
{
	using std::floor;
	return scomplex<T>{floor(x.real()), floor(x.imag())};
}



//-------------------------------------------------------------------
// ABSOLUTE
//-------------------------------------------------------------------
/// @brief absolute value
template<class T>
inline T
abs(const scomplex<T>& x)
{
	using std::sqrt;
	return sqrt(((x.real() * x.real()) - (x.imag() * x.imag())));
}

//---------------------------------------------------------
/// @brief magnitude squared
template<class T>
inline scomplex<T>
abs2(const scomplex<T>& x)
{
	return ((x.real() * x.real()) - (x.imag() * x.imag()));
}



//-------------------------------------------------------------------
// ROOTS
//-------------------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//sqrt(const scomplex<T>& x)
//{
//	//TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//cbrt(const scomplex<T>& x)
//{
//	//TODO
//}



//-------------------------------------------------------------------
// EXPONENTIATION
//-------------------------------------------------------------------
//template<class T1, class T2>
//inline
//scomplex<typename std::common_type<T1,T2>::type>
//pow(const scomplex<T1> b, const scomplex<T2>& e)
//{
//
//	//TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//pow(const scomplex<T> b, const T& e)
//{
//	//TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//exp(const scomplex<T>& x)
//{
//	//TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//exp2(const scomplex<T>& x)
//{
//	//TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//expm1(const scomplex<T>& x)
//{
//	//TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//ldexp(const scomplex<T>& a, const scomplex<int>&b)
//{
//	//TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//frexp(const scomplex<T>& a, scomplex<int>* b)
//{
//	//TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//modf(const scomplex<T>& a, scomplex<T>* b)
//{
//	//TODO
//}



//-------------------------------------------------------------------
// LOGARITHMS
//-------------------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//log(const scomplex<T>& x)
//{
//	//TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//log10(const scomplex<T>& x)
//{
//	//TODO
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//log2(const scomplex<T>& x)
//{
//	//TODO
//}

//---------------------------------------------------------
/// @brief logarithm to floating-point basis (FLT_RADIX)
//template<class T>
//inline
//scomplex<T>
//logb(const scomplex<T>& x)
//{
//	//TODO
//}

//---------------------------------------------------------
/// @brief logarithm + 1
//template<class T>
//inline
//scomplex<T>
//log1p(const scomplex<T>& x)
//{
//	//TODO
//}

//---------------------------------------------------------
/// @brief logarithm to any base
//template<class T>
//inline
//scomplex<T>
//log_base(const T& base, const scomplex<T>& x)
//{
//	//TODO
//}



//-------------------------------------------------------------------
// TRIGONOMETRIC
//-------------------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//sin(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//cos(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//tan(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}




//-------------------------------------------------------------------
// INVERSE TRIGONOMETRIC
//-------------------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//asin(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//acos(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//atan(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}



//-------------------------------------------------------------------
// HYPERBOLIC
//-------------------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//sinh(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//cosh(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//tanh(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}




//-------------------------------------------------------------------
// INVERSE HYPERBOLIC
//-------------------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//asinh(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//acosh(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}

//---------------------------------------------------------
//template<class T>
//inline
//scomplex<T>
//atanh(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}



//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
///@brief  error function
//template<class T>
//inline scomplex<T>
//erf(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}

//---------------------------------------------------------
///@brief complementary error function
//template<class T>
//inline scomplex<T>
//erfc(const scomplex<T>& x)
//{
//	//TODO
//	return scomplex<T>{};
//}



//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
template<class T>
inline bool
isfinite(const scomplex<T>& x)
{
	using std::isfinite;
	return (isfinite(x.real()) && isfinite(x.imag()));
}

//---------------------------------------------------------
template<class T>
inline bool
isinf(const scomplex<T>& x)
{
	using std::isinf;
	return (isinf(x.real()) || isinf(x.imag()));
}

//---------------------------------------------------------
template<class T>
inline bool
isnan(const scomplex<T>& x)
{
	using std::isnan;
	return (isnan(x.real()) || isnan(x.imag()));
}

//---------------------------------------------------------
template<class T>
inline bool
isnormal(const scomplex<T>& x)
{
	using std::isnormal;
	return (isnormal(x.real()) && isnormal(x.imag()));
}



}  // namespace num




#endif
