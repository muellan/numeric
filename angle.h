/*****************************************************************************
 *
 * AM numeric facilities
 *
 * released under MIT license
 *
 * 2008-2013 André Müller
 *
 *****************************************************************************/


#ifndef AM_NUM_NUMERIC_ANGLE_H_
#define AM_NUM_NUMERIC_ANGLE_H_

#include <cmath>
#include <iostream>
#include <utility>

#include "constants.h"
#include "narrowing.h"


namespace num {


/*****************************************************************************
 *
 *
 * BASIC CONVERSIONS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
inline T
deg_to_rad(const T& angle, bool mod2pi = true)
{
	using std::fmod;

	if(mod2pi)
		return fmod(angle*T(pi/180.0), T(2 * pi + 1e-10));
	else
		return (angle * T(pi/180.0));
}

//---------------------------------------------------------
template<class T>
inline T
arcmin2rad(const T& angle, bool mod2pi = true)
{
	return deg_to_rad(angle / T(60), mod2pi);
}

//---------------------------------------------------------
template<class T>
inline T
arcsec_to_rad(const T& angle, bool mod2pi = true)
{
	return deg_to_rad(angle / T(3600), mod2pi);
}


//-------------------------------------------------------------------
template<class T>
inline T
rad_to_deg(const T& angle, bool mod2pi = true)
{
	using std::fmod;

	if(mod2pi)
		return fmod(angle * T(180.0/pi), T(360));
	else
		return (angle * T(180.0/pi));
}

//---------------------------------------------------------
template<class T>
inline T
rad_to_arcmin(const T& angle, bool mod2pi = true)
{
	return (T(60) * rad_to_deg(angle, mod2pi));
}

//---------------------------------------------------------
template<class T>
inline T
rad_to_arcsec(const T& angle, bool mod2pi = true)
{
	return (T(3600) * rad_to_deg(angle, mod2pi));
}







/*****************************************************************************
 *
 *
 * ANGLE UNITS
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
/// @brief degrees full turn
struct degrees_turn
{
	using numeric_type = std::int_least16_t ;
	static constexpr numeric_type value = 360;
};
//---------------------------------------------------------
/// @brief arc minutes full turn
struct arc_mins_turn
{
	using numeric_type = std::int_least16_t ;
	static constexpr numeric_type value = 21600;
};
//---------------------------------------------------------
/// @brief arc seconds full turn
struct arc_secs_turn
{
	using numeric_type = std::int_least32_t ;
	static constexpr numeric_type value = 1296000;
};


//-------------------------------------------------------------------
/// @brief radians full turn
struct radians_turn
{
	using numeric_type = real_t;
	static constexpr numeric_type value = (2 * pi);
};


//-------------------------------------------------------------------
/// @brief grads/gons full turn
struct gons_turn
{
	using numeric_type = std::int_least16_t ;
	static constexpr numeric_type value = 400;
};
//---------------------------------------------------------
/// @brief grads/gons cs full turn
struct gon_cs_turn
{
	using numeric_type = std::int_least32_t ;
	static constexpr numeric_type value = 40000;
};
//---------------------------------------------------------
/// @brief grads/gons ccs full turn
struct gon_ccs_turn
{
	using numeric_type = std::int_least32_t ;
	static constexpr numeric_type value = 4000000;
};








/*****************************************************************************
 *
 * @brief represents an angle
 *        supports automatic angle unit conversions
 *        supports constexpr compile-time evaluation
 *
 * @tparam Turn        policy class providing angle value of a full circle
 * @tparam NumberType  arithmetic type used for storing the angle value
 *
 *****************************************************************************/
template<class Turn, class NumberType>
class angle
{
	//---------------------------------------------------------
	template<class T>
	struct conversion {
		using type =
			typename std::common_type<typename T::numeric_type,NumberType>::type;
	};

	static_assert(is_number<NumberType>::value, "angle<U,T>: T must be a number");

public:
	//---------------------------------------------------------------
	// TYPES
	//---------------------------------------------------------------
	using turn_type   = Turn;
	using numeric_type = NumberType;


	//---------------------------------------------------------------
	// CONSTANTS
	//---------------------------------------------------------------
	static constexpr typename turn_type::numeric_type full_turn() {
		return (turn_type::value);
	}


	//---------------------------------------------------------------
	// CONSTRUCTION / DESTRUCTION
	//---------------------------------------------------------------
	///@brief no empty contructor, angle has to be initialized with a value
	angle() = delete;

	//-----------------------------------------------------
	/// @brief implicit conversion from simple values is not allowed
	explicit constexpr
	angle(numeric_type a):
		v_{a}
	{}
	//-----------------------------------------------------
	/// @brief   implicit converions of angles is allowed and safe
	/// @details may cause numerical problems with floating point arithmetic
	template<class T, class V, class = typename
		std::enable_if<is_non_narrowing<numeric_type,V>::value>::type>
	constexpr
	angle(const angle<T,V>& a):
		v_{a.as<turn_type>()}
	{}

	//-----------------------------------------------------
	constexpr
	angle(const angle&) = default;

	//---------------------------------------------------------------
	template<class T, class V, class = typename
		std::enable_if<is_non_narrowing<numeric_type,V>::value>::type>
	angle&
	operator = (const angle<T,V>& a) {
		v_ = a.as<turn_type>();
		return *this;
	}


	//---------------------------------------------------------------
	/// @brief maps the stored value to the range [0, full_turn()]
	angle&
	wrap() {
		using std::fmod;
		if(v_ < numeric_type(0)) {
			v_ = -v_;
		}
		if(v_ > full_turn()) {
			v_ = fmod(v_, full_turn());
		}
		return *this;
	}


	//---------------------------------------------------------------
	// ASSIGNING ARITHMETIC OPERATIONS
	//---------------------------------------------------------------
	template<class T, class V, class = typename
		std::enable_if<is_non_narrowing<numeric_type,V>::value>::type>
	angle&
	operator += (const angle<T,V>& a) {
		v_ += a.as<turn_type>();
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class V, class = typename
		std::enable_if<is_non_narrowing<numeric_type,V>::value>::type>
	angle&
	operator -= (const angle<T,V>& a) {
		v_ += a.as<turn_type>();
		return *this;
	}
	//-----------------------------------------------------
	angle&
	operator *= (numeric_type factor) {
		v_ *= factor;
		return *this;
	}
	//-----------------------------------------------------
	angle&
	operator /= (numeric_type factor) {
		v_ /= factor;
		return *this;
	}


	//---------------------------------------------------------------
	// ARITHMETIC OPERATIONS
	//---------------------------------------------------------------
	template<class T, class V, class = typename
		std::enable_if<is_non_narrowing<numeric_type,V>::value>::type>
	constexpr angle
	operator + (const angle<T,V>& a) const {
		return angle{v_ + a.as<turn_type>()};
	}
	//-----------------------------------------------------
	template<class T, class V, class = typename
		std::enable_if<is_non_narrowing<numeric_type,V>::value>::type>
	constexpr angle
	operator - (const angle<T,V>& a) const {
		return angle{v_ - a.as<turn_type>()};
	}
	//-----------------------------------------------------
	constexpr angle
	operator * (numeric_type factor) const {
		return angle{v_ * factor};
	}
	//-----------------------------------------------------
	constexpr angle
	operator / (numeric_type factor) const {
		return angle{v_ / factor};
	}

	//-----------------------------------------------------
	/// @brief power
	constexpr angle
	operator ^ (numeric_type factor) const {
		return angle{std::pow(v_,factor)};
	}

	//-----------------------------------------------------
	/// @brief pre-multiplication
	inline friend constexpr angle
	operator * (numeric_type factor, const angle& a) {
		return angle{factor * a.v_};
	}


	//---------------------------------------------------------------
	// inversion (works only on signed domains)
	//---------------------------------------------------------------
	template<class V = numeric_type>
	constexpr typename std::enable_if<!std::is_unsigned<V>::value, angle>::type
	operator - () const {
		return angle{-v_};
	}


	//---------------------------------------------------------------
	// increment/decrement operations for integral angles
	//---------------------------------------------------------------
	template<class V = numeric_type>
	typename std::enable_if<std::is_integral<V>::value,angle>::type
	operator ++ () {
		++v_;
		return *this;
	}
	//-----------------------------------------------------
	template<class V = numeric_type>
	typename std::enable_if<std::is_integral<V>::value,angle>::type
	operator ++ (int) {
		auto old = angle{*this};
		operator++();
		return old;
	}
	//-----------------------------------------------------
	template<class V = numeric_type>
	typename std::enable_if<std::is_integral<V>::value,angle>::type
	operator -- () {
		--v_;
		return *this;
	}
	//-----------------------------------------------------
	template<class V = numeric_type>
	typename std::enable_if<std::is_integral<V>::value,angle>::type
	operator -- (int) {
		auto old = angle{*this};
		operator--();
		return old;
	}


	//---------------------------------------------------------------
	// COMPARISON
	//---------------------------------------------------------------
	template<class T, class V, class V2 = numeric_type>
	constexpr
	typename std::enable_if<std::is_integral<V>::value,
		typename std::enable_if<std::is_integral<V2>::value,bool>::type
	>::type
	operator == (const angle<T,V>& other) {
		return (v_ == other.as<turn_type>());
	}
	//-----------------------------------------------------
	template<class T, class V, class V2 = numeric_type>
	constexpr
	typename std::enable_if<std::is_integral<V>::value,
		typename std::enable_if<std::is_integral<V2>::value,bool>::type
	>::type
	operator != (const angle<T,V>& other) {
		return (v_ != other.as<turn_type>());
	}

	//-----------------------------------------------------
	template<class T, class V>
	constexpr bool
	operator < (const angle<T,V>& other) {
		return (v_ < other.as<turn_type>());
	}
	//-----------------------------------------------------
	template<class T, class V>
	constexpr bool
	operator > (const angle<T,V>& other) {
		return (v_ > other.as<turn_type>());
	}
	//-----------------------------------------------------
	template<class T, class V>
	constexpr bool
	operator <= (const angle<T,V>& other) {
		return (v_ <= other.as<turn_type>());
	}
	//-----------------------------------------------------
	template<class T, class V>
	constexpr bool
	operator >= (const angle<T,V>& other) {
		return (v_ >= other.as<turn_type>());
	}


	//---------------------------------------------------------------
	// TODO num::angle::as<> get rid of in favor of angle_cast<>
	template<class OutTurn, class = typename
		std::enable_if<std::is_same<turn_type,OutTurn>::value>::type>
	constexpr numeric_type
	as() const {
		return v_;
	}
	//-----------------------------------------------------
	template<class OutTurn, class = typename
		std::enable_if<!std::is_same<turn_type,OutTurn>::value>::type>
	constexpr typename conversion<OutTurn>::type
	as() const {
		using res_t = typename conversion<OutTurn>::type;
		return ( (res_t(OutTurn::value) / res_t(full_turn())) * v_);
	}

	//---------------------------------------------------------------
	template<class OutTurn>
	inline friend constexpr typename conversion<OutTurn>::type
	angle_cast(const angle& a)	{
		return (a.as<OutTurn>());
	}

	//-----------------------------------------------------
	constexpr typename conversion<radians_turn>::type
	rad() const {
		return as<radians_turn>();
	}
	//-----------------------------------------------------
	constexpr typename conversion<degrees_turn>::type
	deg() const {
		return as<degrees_turn>();
	}
	//-----------------------------------------------------
	constexpr typename conversion<gons_turn>::type
	gon() const {
		return as<gons_turn>();
	}

private:


	//---------------------------------------------------------------
	numeric_type v_ = 0;
};








/*****************************************************************************
 *
 *
 * CONVENIENCE DEFINITIONS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class V>
using radians = angle<radians_turn,V>;

//---------------------------------------------------------
template<class V>
using degrees = angle<degrees_turn,V>;

template<class V>
using arcmins = angle<arc_mins_turn,V>;

template<class V>
using arcsecs = angle<arc_secs_turn,V>;

//---------------------------------------------------------
template<class V>
using gons = angle<gons_turn,V>;

template<class V>
using gon_cs = angle<gon_cs_turn,V>;

template<class V>
using gon_ccs = angle<gon_ccs_turn,V>;


//-------------------------------------------------------------------
using rad  = angle<radians_turn,real_t>;
using radf = angle<radians_turn,float>;
using radd = angle<radians_turn,double>;

using deg  = angle<degrees_turn,real_t>;
using degf = angle<degrees_turn,float>;
using degd = angle<degrees_turn,double>;
using degi = angle<degrees_turn,int>;

using gon  = angle<gons_turn,real_t>;
using gonf = angle<gons_turn,float>;
using gond = angle<gons_turn,double>;
using goni = angle<gons_turn,int>;







/*****************************************************************************
 *
 *
 * LITERALS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
constexpr degrees<real_t> operator"" _deg (long double x) {
    return degrees<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr degrees<real_t> operator"" _deg (unsigned long long int x) {
    return degrees<real_t>{real_t(x)};
}


//-------------------------------------------------------------------
constexpr radians<real_t> operator"" _rad (long double x) {
    return radians<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr radians<real_t> operator"" _rad (unsigned long long int x) {
    return radians<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr radians<real_t> operator"" _pi_rad(long double x) {
	return radians<real_t>{real_t(x * pi)};
}
//---------------------------------------------------------
constexpr radians<real_t> operator"" _pi_rad(unsigned long long int x) {
	return radians<real_t>{real_t(x * pi)};
}

//-------------------------------------------------------------------
constexpr gons<real_t> operator"" _gon (long double x) {
    return gons<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr gons<real_t> operator"" _gon (unsigned long long int x) {
    return gons<real_t>{real_t(x)};
}






/*****************************************************************************
 *
 *
 * I/O
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// I/O
//-------------------------------------------------------------------
template<class CharT, class Traits, class U, class T>
inline std::basic_ostream<CharT,Traits>&
operator << (std::basic_ostream<CharT,Traits>& os, const angle<U,T>& r)
{
	return (os << angle_cast<U>(r));
}


//-------------------------------------------------------------------
// pretty printing
//-------------------------------------------------------------------
template<class CharT, class Traits, class V>
inline std::basic_ostream<CharT,Traits>&
print(const angle<radians_turn,V>& a, std::basic_ostream<CharT,Traits>& os)
{
	return (os << a << "rad");
}

//---------------------------------------------------------
template<class CharT, class Traits, class V>
inline std::basic_ostream<CharT,Traits>&
print(const angle<degrees_turn,V>& a, std::basic_ostream<CharT,Traits>& os)
{
	return (os << a << "°");
}

//---------------------------------------------------------
template<class CharT, class Traits, class V>
inline std::basic_ostream<CharT,Traits>&
print(const angle<gons_turn,V>& a, std::basic_ostream<CharT,Traits>& os)
{
	return (os << a << "gon");
}






/*****************************************************************************
 *
 *
 * TRAITS
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T, class... R>
struct is_angle
{
	static constexpr bool value =
		is_angle<T>::value && is_angle<R...>::value;
};

//---------------------------------------------------------
template<class T>
struct is_angle<T>
{
	static constexpr bool value = false;
};

//-----------------------------------------------------
template<class U, class T>
struct is_angle<angle<U,T>>
{
	static constexpr bool value = true;
};






/*****************************************************************************
 *
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
/// @brief maps the input angle to the turn range
template<class U, class T>
inline constexpr angle<U,T>
wrapped(angle<U,T> a)
{
	return angle<U,T>{a.wrap()};
}


//-------------------------------------------------------------------
/// @brief returns full_turn - wrapped(angle)
template<class U, class T>
inline angle<U,T>
turn_remainder(angle<U,T> a)
{
	auto r = angle<U,T>{a.full_turn() - angle_cast<U>(a)};
	r.wrap();
	return r;
}







/*****************************************************************************
 *
 *
 * TRIGONOMETRY
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
// TRIGONOMETRIC FUNCTIONS
//-------------------------------------------------------------------
template<class T, class V>
inline typename std::common_type<V,real_t>::type
sin(const angle<T,V>& r)
{
	return ::sin(r.rad());
}
//-----------------------------------------------------
template<class T, class V>
inline typename std::common_type<V,real_t>::type
cos(const angle<T,V>& r)
{
	return ::cos(r.rad());
}
//-----------------------------------------------------
template<class T, class V>
inline typename std::common_type<V,real_t>::type
tan(const angle<T,V>& r)
{
	return ::tan(r.rad());
}



//-------------------------------------------------------------------
// HYPERBOLIC FUNCTIONS
//-------------------------------------------------------------------
template<class T, class V>
inline typename std::common_type<V,real_t>::type
sinh(const angle<T,V>& r)
{
	return ::sinh(r.rad());
}
//-----------------------------------------------------
template<class T, class V>
inline typename std::common_type<V,real_t>::type
cosh(const angle<T,V>& r)
{
	return ::cosh(r.rad());
}
//-----------------------------------------------------
template<class T, class V>
inline typename std::common_type<V,real_t>::type
tanh(const angle<T,V>& r)
{
	return ::tanh(r.rad());
}



//-------------------------------------------------------------------
// INVERSE TRIGONOMETRIC FUNCTIONS
//-------------------------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
radians_asin(T v)
{
	using std::asin;
	using res_t = typename std::common_type<T,real_t>::type;
	return radians<res_t>{res_t(asin(v))};
}
//-----------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
radians_acos(T v)
{
	using std::acos;
	using res_t = typename std::common_type<T,real_t>::type;
	return radians<res_t>{res_t(acos(v))};
}
//-----------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
radians_atan(T v)
{
	using std::atan;
	using res_t = typename std::common_type<T,real_t>::type;
	return radians<res_t>{res_t(atan(v))};
}
//-----------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
radians_atan2(T x, T y)
{
	using std::atan2;
	using res_t = typename std::common_type<T,real_t>::type;
	return radians<res_t>{res_t(atan2(x,y))};
}



//-------------------------------------------------------------------
// INVERSE HYPERBOLIC FUNCTIONS
//-------------------------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
radians_asinh(T v)
{
	using std::asinh;
	using res_t = typename std::common_type<T,real_t>::type;
	return radians<res_t>{res_t(asinh(v))};
}
//-----------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
radians_acosh(T v)
{
	using std::acosh;
	using res_t = typename std::common_type<T,real_t>::type;
	return radians<res_t>{res_t(acosh(v))};
}
//-----------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
radians_atanh(T v)
{
	using std::atanh;
	using res_t = typename std::common_type<T,real_t>::type;
	return radians<res_t>{res_t(atanh(v))};
}


}  // namespace num


#endif
