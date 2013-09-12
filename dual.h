#ifndef AM_NUMERIC_DUAL_H_
#define AM_NUMERIC_DUAL_H_

#include <cmath>
#include <cfloat>

#include "concepts.h"
#include "narrowing.h"



namespace num {


/*****************************************************************************
 *
 * @brief
 * represents a dual number r + e * d where
 * r is the real part,
 * d is the dual part and
 * e is the dual unit with e*e = 0
 *
 *****************************************************************************/
template<class NumberType>
class dual
{
public:

	static_assert(is_number<NumberType>::value,
		"dual<T>: T must be a number");


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
	dual() = default;

	//-----------------------------------------------------
	/// @brief
	explicit constexpr
	dual(const value_type& a):
		n_{a,value_type(0)}
	{}

	//-----------------------------------------------------
	/// @brief
	template<class T, class = typename
		std::enable_if<is_number<T>::value>::type>
	explicit constexpr
	dual(const T& a):
		n_{value_type(a),value_type(0)}
	{
		AM_CHECK_NARROWING(value_type,T)
	}

	//-----------------------------------------------------
	/// @brief
	constexpr
	dual(const value_type& r, const value_type& d):
		n_{r, d}
	{}

	//-----------------------------------------------------
	/// @brief
	template<class T1, class T2, class = typename
		std::enable_if<is_number<T1,T2>::value>::type>
	explicit constexpr
	dual(const T1& r, const T2& d):
		n_{value_type(r), value_type(d)}
	{
		AM_CHECK_NARROWING2(value_type,T1,T2)
	}

	//-----------------------------------------------------
	constexpr
	dual(const dual&) = default;


	//---------------------------------------------------------------
	// ASSIGNMENT
	//---------------------------------------------------------------
	dual&
	operator = (const dual&) = default;

	//-----------------------------------------------------
	template<class T, class =
		typename std::enable_if<is_number<T>::value>::type>
	dual&
	operator = (const dual<T>& d)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] = d.n_[0];
		n_[1] = d.n_[1];
		return *this;
	}

	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_number<T>::value>::type>
	dual&
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
	dual&
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
	dual&
	conjugate() {
		n_[1] = -n_[1];
		return *this;
	}

	//---------------------------------------------------------
	dual&
	negate() noexcept {
		n_[0] = -n_[0];
		n_[1] = -n_[1];
		return *this;
	}


	//---------------------------------------------------------------
	// dual (op)= number
	//---------------------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_number<T>::value>::type>
	dual&
	operator += (const T& v)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] += v;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_number<T>::value>::type>
	dual&
	operator -= (const T& v) {
		n_[0] -= v;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_number<T>::value>::type>
	dual&
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
	dual&
	operator /= (const T& v)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] /= v;
		n_[1] /= v;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename
		std::enable_if<is_number<T>::value>::type>
	dual&
	operator ^= (const T& e)
	{
		AM_CHECK_NARROWING(value_type,T)

		using std::pow;

		auto a_ya_1 = pow(n_[0] , e-1);

		n_[0] *= a_ya_1;
		n_[1] *= e * a_ya_1;

		return *this;
	}


	//---------------------------------------------------------------
	// increment / decrement
	//---------------------------------------------------------------
	dual&
	operator ++ ()
	{
		++n_[0];
		return *this;
	}
	//-----------------------------------------------------
	dual&
	operator -- ()
	{
		--n_[0];
		return *this;
	}

	//-----------------------------------------------------
	dual
	operator ++ (int)
	{
		auto old = *this;
		++*this;
		return old;
	}
	//-----------------------------------------------------
	dual
	operator -- (int)
	{
		auto old = *this;
		--*this;
		return old;
	}


	//---------------------------------------------------------------
	// dual (op)= dual with different value_type
	//---------------------------------------------------------------
	template<class T>
	dual&
	operator += (const dual<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] += o.real();
		n_[1] += o.imag();
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	dual&
	operator -= (const dual<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[0] -= o.real();
		n_[1] -= o.imag();
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	dual&
	operator *= (const dual<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[1] = (n_[0] * o.imag()) + (o.real() * n_[1]);
		n_[0] *= o.real();
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	dual&
	operator /= (const dual<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[1] = ((n_[1] * o.real()) - (n_[0] * o.imag())) / (o.real() * o.real());
		n_[0] /= o.real();
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	dual&
	operator ^= (const dual<T>& y)
	{
		AM_CHECK_NARROWING(value_type,T)

		using std::pow;
		using std::log;

		auto a_ya_1 = pow(n_[0] , y.real()-1);

		n_[0] *= a_ya_1 * (1 + (y.imag() * log(n_[0])));
		n_[1] *= y.real() * a_ya_1;

		return *this;
	}

	//-----------------------------------------------------
	template<class T>
	dual&
	times_conj(const dual<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[1] = (n_[0] * (-o.imag())) + (o.real() * n_[1]);
		n_[0] *= o.real();
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	dual&
	conj_times(const dual<T>& o)
	{
		AM_CHECK_NARROWING(value_type,T)

		n_[1] = (n_[0] * o.imag()) + (o.real() * (-n_[1]));
		n_[0] *= o.real();
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
dual<typename std::common_type<T1,T2>::type>
make_dual(const T1& a, const T2& b)
{
	return dual<typename std::common_type<T1,T2>::type>{a,b};
}

//---------------------------------------------------------------
template<class T, class = typename
	std::enable_if<is_number<T>::value,T>::type>
inline
dual<T>
make_dual(const T& x)
{
	return dual<T>{x, T(0)};
}



//-------------------------------------------------------------------
// I/O
//-------------------------------------------------------------------
template<class Istream, class T>
inline Istream&
operator >> (Istream& is, dual<T>& x)
{
	return (is >> x.real() >> x.imag());
}

//---------------------------------------------------------
template<class Ostream, class T>
inline Ostream&
operator << (Ostream& os, const dual<T>& x)
{
	return (os << x.real() << " " << x.imag() );
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const dual<T>& d)
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
real(const dual<T>& d) noexcept -> decltype(d.real())
{
	return d.real();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr auto
imag(const dual<T>& d) noexcept -> decltype(d.imag())
{
	return d.imag();
}



//-------------------------------------------------------------------
template<class T>
inline constexpr
dual<T>
conj(const dual<T>& x)
{
	return dual<T>{x.real(), -x.imag()};
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
operator == (const dual<T1>& a, const dual<T2>& b)
{
	return ((a.real() == b.real()) && (a.imag() == b.imag()));
}

//---------------------------------------------------------
template<class T1, class T2>
inline bool
operator != (const dual<T1>& a, const dual<T2>& b)
{
	return ((a.real() != b.real()) || (a.imag() != b.imag()));
}


//-------------------------------------------------------------------
template<class T1, class T2, class T3 = typename std::common_type<T1,T2>::type>
inline constexpr bool
approx_equal(const dual<T1>& a, const dual<T2>& b,
	const T3& tolerance = epsilon<T3>::value)
{
	return (approx_equal(a.real(), b.real(), tolerance) &&
		    approx_equal(a.imag(), b.imag(), tolerance) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_1(const dual<T>& x, const T& tolerance = epsilon<T>::value)
{
	return (
		approx_1(x.real(), tolerance) &&
		approx_0(x.imag(), tolerance) );
}

//---------------------------------------------------------
template<class T>
inline constexpr bool
approx_0(const dual<T>& x, const T& tolerance = epsilon<T>::value)
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
operator > (const dual<T1>& x, const T2& r)
{
	return (x.real() > r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator > (const T2& r, const dual<T1>& x)
{
	return (r > x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator >= (const dual<T1>& x, const T2& r)
{
	return (x.real() >= r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator >= (const T2& r, const dual<T1>& x)
{
	return (r >= x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator < (const dual<T1>& x, const T2& r)
{
	return (x.real() < r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator < (const T2& r, const dual<T1>& x)
{
	return (r < x.real());
}

//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator <= (const dual<T1>& x, const T2& r)
{
	return (x.real() <= r);
}
//---------------------------------------------------------
template<class T1, class T2, class = typename
	std::enable_if<is_number<T2>::value>::type>
inline bool
operator <= (const T2& r, const dual<T1>& x)
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
dual<typename std::common_type<T1,T2>::type>
operator + (const dual<T1> x, const dual<T2>& y)
{
	return dual<typename std::common_type<T1,T2>::type>{
		x.real() + y.real(), x.imag() + y.imag()};
}

//---------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator + (const dual<T> x, const T& y)
{
	return dual<T>{x.real() + y, x.imag() + y};
}



//-------------------------------------------------------------------
// SUBTRACTION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
dual<typename std::common_type<T1,T2>::type>
operator - (const dual<T1> x, const dual<T2>& y)
{
	return dual<typename std::common_type<T1,T2>::type>{
		x.real() - y.real(), x.imag() - y.imag()};
}

//---------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator - (const dual<T> x, const T& y)
{
	return dual<T>{x.real() - y, x.imag() - y};
}



//-------------------------------------------------------------------
// MULTIPLIPCATION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
dual<typename std::common_type<T1,T2>::type>
operator * (const dual<T1> x, const dual<T2>& y)
{
	return dual<typename std::common_type<T1,T2>::type>{
		x.real() * y.real(),
		(x.real() * y.imag()) + (y.real() * x.imag()) };
}

//---------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator * (const dual<T> x, const T& y)
{
	return dual<T>{x.real() * y, x.imag() * y};
}



//-------------------------------------------------------------------
// DIVISION
//-------------------------------------------------------------------
template<class T1, class T2>
inline constexpr
dual<typename std::common_type<T1,T2>::type>
operator / (const dual<T1> x, const dual<T2>& y)
{
	return dual<typename std::common_type<T1,T2>::type>{
		x.real() / y.real(),
		((x.imag() * y.real()) - (x.real() * y.imag())) / (y.real() * y.real())
	};
}

//---------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator / (const dual<T> x, const T& y)
{
	return dual<T>{x.real() / y, x.imag() / y};
}



//-------------------------------------------------------------------
// EXPONENTIATION
//-------------------------------------------------------------------
template<class T1, class T2>
inline
dual<typename std::common_type<T1,T2>::type>
operator ^ (const dual<T1> b, const dual<T2>& e)
{
	return pow(b,e);
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
operator ^ (const dual<T> b, const T& e)
{
	return pow(b,e);
}



//-------------------------------------------------------------------
// INVERSION
//-------------------------------------------------------------------
template<class T>
inline constexpr
dual<T>
operator - (const dual<T> x)
{
	return dual<T>{-x.real(), -x.imag()};
}



//-------------------------------------------------------------------
// SPECIAL MULTIPLICATIONS
//-------------------------------------------------------------------
template<class T1, class T2>
inline dual<typename std::common_type<T1,T2>::type>
times_conj(const dual<T1>& x, const dual<T2>& y)
{
	return dual<typename std::common_type<T1,T2>::type>{
		x.real() * y.real(),
		(x.real() * (-y.imag())) + (y.real() * x.imag()) };
}

//---------------------------------------------------------
template<class T1, class T2>
inline dual<typename std::common_type<T1,T2>::type>
conj_times(const dual<T1>& x, const dual<T2>& y)
{
	return dual<typename std::common_type<T1,T2>::type>{
		x.real() * y.real(),
		(x.real() * y.imag()) + (y.real() * (-x.imag())) };
}






/*****************************************************************************
 *
 *
 * FUNCTIONS
 *
 * @note f(r + e*d) = f(r) + d*f'(r) * e
 *       where e is the dual unit, r the real part and d the dual part
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
template<class T>
inline
dual<T>
ceil(const dual<T>& x)
{
	using std::ceil;
	return dual<T>{ceil(x.real()), ceil(x.imag())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
floor(const dual<T>& x)
{
	using std::floor;
	return dual<T>{floor(x.real()), floor(x.imag())};
}



//-------------------------------------------------------------------
// ABSOLUTE
//-------------------------------------------------------------------
/// @brief absolute value
template<class T>
inline T
abs(const dual<T>& x)
{
	using std::abs;
	return abs(x.real());
}

//---------------------------------------------------------
/// @brief magnitude squared
template<class T>
inline dual<T>
abs2(const dual<T>& x)
{
	return dual<T>{(x.real() * x.real()), 0};
}



//-------------------------------------------------------------------
// ROOTS
//-------------------------------------------------------------------
template<class T>
inline
dual<T>
sqrt(const dual<T>& x)
{
	using std::sqrt;
	auto sqrt_r = sqrt(x.real());

	return dual<T>{sqrt_r, x.imag() / (T(2) * sqrt_r)};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
cbrt(const dual<T>& x)
{
	using std::cbrt;
	auto cbrt_r = cbrt(x.real());

	return dual<T>{cbrt_r, x.imag() / (T(3) * cbrt_r*cbrt_r)};
}



//-------------------------------------------------------------------
// EXPONENTIATION
//-------------------------------------------------------------------
template<class T1, class T2>
inline
dual<typename std::common_type<T1,T2>::type>
pow(const dual<T1> b, const dual<T2>& e)
{
	using std::pow;
	using std::log;

	auto ba_ea_1 = pow(b.real(), e.real()-1);

	return dual<typename std::common_type<T1,T2>::type>{
		b.real() * ba_ea_1 * (1 + (e.imag() * log(b.real()))),
		b.imag() * e.real() * ba_ea_1 };
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
pow(const dual<T> b, const T& e)
{
	using std::pow;

	auto ba_e_1 = pow(b.real(), e - 1);

	return dual<T>{b.real() * ba_e_1, b.imag() * e * ba_e_1};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
exp(const dual<T>& x)
{
	using std::exp;
	auto expr = exp(x.real());
	return dual<T>{expr, x.imag() * expr};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
exp2(const dual<T>& x)
{
	using std::exp2;
	auto exp2r = exp2(x.real());

	return dual<T>{exp2r,
		x.imag() * exp2r * T(0.69314718055994530941723212145817656807550013436026)};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
expm1(const dual<T>& x)
{
	using std::expm1;
	using std::exp;
	return dual<T>{expm1(x.real()), x.imag() * exp(x.real())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
ldexp(const dual<T>& a, const dual<int>&b)
{
	using std::ldexp;
	return dual<T>{ldexp(a.real(), b.real()), ldexp(a.imag(), b.imag())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
frexp(const dual<T>& a, dual<int>* b)
{
	using std::frexp;
	return dual<T>{
		frexp(a.real(), std::addressof(b->real())),
		frexp(a.imag(), std::addressof(b->imag()))};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
modf(const dual<T>& a, dual<T>* b)
{
	using std::modf;
	return dual<T>{
		modf(a.real(), std::addressof(b->real())),
		modf(a.imag(), std::addressof(b->imag()))};
}



//-------------------------------------------------------------------
// LOGARITHMS
//-------------------------------------------------------------------
template<class T>
inline
dual<T>
log(const dual<T>& x)
{
	using std::log;
	return dual<T>{log(x.real()), x.imag() / x.real()};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
log10(const dual<T>& x)
{
	using std::log10;
	return dual<T>{
		log10(x.real()), x.imag() /
			(x.real() * T(2.3025850929940456840179914546843642076011014886288))};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
log2(const dual<T>& x)
{
	using std::log2;
	return dual<T>{
		log2(x.real()), x.imag() /
			(x.real() * T(0.69314718055994530941723212145817656807550013436026))};
}

//---------------------------------------------------------
/// @brief logarithm to floating-point basis (FLT_RADIX)
template<class T>
inline
dual<T>
logb(const dual<T>& x)
{
	using std::logb;
	using std::log;
	return dual<T>{logb(x.real()), x.imag() / (x.real() * log(FLT_RADIX))};
}

//---------------------------------------------------------
/// @brief logarithm + 1
template<class T>
inline
dual<T>
log1p(const dual<T>& x)
{
	using std::log1p;
	return dual<T>{log1p(x.real()), x.imag() / x.real()};
}

//---------------------------------------------------------
/// @brief logarithm to any base
template<class T>
inline
dual<T>
log_base(const T& base, const dual<T>& x)
{
	using std::log;
	auto logbase_inv = T(1)/log(base);
	return dual<T>{
		log(x.real()) * logbase_inv,
		(x.imag() / x.real()) * logbase_inv};
}



//-------------------------------------------------------------------
// TRIGONOMETRIC
//-------------------------------------------------------------------
template<class T>
inline
dual<T>
sin(const dual<T>& x)
{
	using std::sin;
	using std::cos;
	return dual<T>{sin(x.real()), x.imag() * cos(x.real())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
cos(const dual<T>& x)
{
	using std::sin;
	using std::cos;
	return dual<T>{cos(x.real()), -x.imag() * sin(x.real())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
tan(const dual<T>& x)
{
	using std::cos;
	using std::tan;
	auto cosr2 = cos(x.real());
	cosr2 *= cosr2;
	return dual<T>{tan(x.real()), x.imag() / cosr2};
}




//-------------------------------------------------------------------
// INVERSE TRIGONOMETRIC
//-------------------------------------------------------------------
template<class T>
inline
dual<T>
asin(const dual<T>& x)
{
	using std::asin;
	using std::sqrt;
	return dual<T>{asin(x.real()), x.imag() / sqrt(1 - (x.real()*x.real()))};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
acos(const dual<T>& x)
{
	using std::acos;
	using std::sqrt;
	return dual<T>{acos(x.real()), -x.imag() / sqrt(1 - (x.real()*x.real()))};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
atan(const dual<T>& x)
{
	using std::atan;
	return dual<T>{atan(x.real()), x.imag() / (1 + (x.real() * x.real()))};
}



//-------------------------------------------------------------------
// HYPERBOLIC
//-------------------------------------------------------------------
template<class T>
inline
dual<T>
sinh(const dual<T>& x)
{
	using std::sinh;
	using std::cosh;
	return dual<T>{sinh(x.real()), x.imag() * cosh(x.real())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
cosh(const dual<T>& x)
{
	using std::sinh;
	using std::cosh;
	return dual<T>{cosh(x.real()), x.imag() * sinh(x.real())};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
tanh(const dual<T>& x)
{
	using std::cosh;
	using std::tanh;
	auto coshr2 = cosh(x.real());
	coshr2 *= coshr2;
	return dual<T>{tanh(x.real()), x.imag() / coshr2};
}




//-------------------------------------------------------------------
// INVERSE HYPERBOLIC
//-------------------------------------------------------------------
template<class T>
inline
dual<T>
asinh(const dual<T>& x)
{
	using std::asinh;
	using std::sqrt;
	return dual<T>{asinh(x.real()), x.imag() / sqrt((x.real()*x.real()) + 1)};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
acosh(const dual<T>& x)
{
	using std::acosh;
	using std::sqrt;
	return dual<T>{acosh(x.real()), -x.imag() / sqrt((x.real()*x.real()) - 1)};
}

//---------------------------------------------------------
template<class T>
inline
dual<T>
atanh(const dual<T>& x)
{
	using std::atanh;
	return dual<T>{atanh(x.real()), x.imag() / (1 - (x.real() * x.real()))};
}



//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
///@brief  error function
template<class T>
inline dual<T>
erf(const dual<T>& x)
{
	using std::erf;
	using std::exp;
	return dual<T>{erf(x.real()), x.imag() * exp(-x.real()*x.real()) *
		T(1.1283791670955125738961589031215451716881012586580) };
}

//---------------------------------------------------------
///@brief complementary error function
template<class T>
inline dual<T>
erfc(const dual<T>& x)
{
	using std::erfc;
	using std::exp;
	return dual<T>{erfc(x.real()), -x.imag() * exp(-x.real()*x.real()) *
		T(1.1283791670955125738961589031215451716881012586580) };
}

//---------------------------------------------------------
///@brief gamma function
//template<class T>
//inline dual<T>
//tgamma(const dual<T>& x)
//{
//	using std::tgamma;
//	auto tgamma_r = tgamma(x.real());
//	return dual<T>{tgamma_r, x.imag() * tgamma_r * digamma(x.real())};
//}

//---------------------------------------------------------
///@brief log-gamma function
//template<class T>
//inline dual<T>
//lgamma(const dual<T>& x)
//{
//	using std::lgamma;
//	return dual<T>{lgamma(x.real()), x.imag() * digamma(x.real())};
//}



//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
template<class T>
inline bool
isfinite(const dual<T>& x)
{
	using std::isfinite;
	return (isfinite(x.real()) && isfinite(x.imag()));
}

//---------------------------------------------------------
template<class T>
inline bool
isinf(const dual<T>& x)
{
	using std::isinf;
	return (isinf(x.real()) || isinf(x.imag()));
}

//---------------------------------------------------------
template<class T>
inline bool
isnan(const dual<T>& x)
{
	using std::isnan;
	return (isnan(x.real()) || isnan(x.imag()));
}

//---------------------------------------------------------
template<class T>
inline bool
isnormal(const dual<T>& x)
{
	using std::isnormal;
	return (isnormal(x.real()) && isnormal(x.imag()));
}



}  // namespace num




#endif
