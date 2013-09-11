#ifndef AM_NUMERIC_ANGLE_H_
#define AM_NUMERIC_ANGLE_H_

#include <cmath>
#include <utility>

#include "constants.h"
#include "narrowing.h"




namespace num {


/*****************************************************************************
 *
 *
 * ANGLE UNITS
 *
 *
 *****************************************************************************/


//-------------------------------------------------------------------
/// @brief degrees full turn
template<class T>
struct degrees_turn
{
	using type = T;
	static constexpr type value = 360;
};
//---------------------------------------------------------
/// @brief arc minutes full turn
template<class T>
struct arcmins_turn
{
	using type = T;
	static constexpr type value = 21600;
};
//---------------------------------------------------------
/// @brief arc seconds full turn
template<class T>
struct arcsecs_turn
{
	using type = T;
	static constexpr type value = 1296000;
};


//-------------------------------------------------------------------
/// @brief radians full turn
template<class T>
struct radians_turn
{
	using type = T;
	static constexpr type value = (2 * pi);
};


//-------------------------------------------------------------------
/// @brief grads/gons full turn
template<class T>
struct gons_turn
{
	using type = T;
	static constexpr type value = 400;
};
//---------------------------------------------------------
/// @brief grads/gons cs full turn
template<class T>
struct gon_cs_turn
{
	using type = T;
	static constexpr type value = 40000;
};
//---------------------------------------------------------
/// @brief grads/gons ccs full turn
template<class T>
struct gon_ccs_turn
{
	using type = T;
	static constexpr type value = 4000000;
};








/*****************************************************************************
 *
 * @brief represents an angle
 *        supports automatic angle unit conversions
 *        supports constexpr compile-time evaluation
 *
 * @tparam Turn  policy class providing angle value of a full circle
 *               and the arithmetic type used for storing the angle value
 *
 *****************************************************************************/
template<class Turn>
class angle
{
	//---------------------------------------------------------
	template<class T>
	struct conversion {
		using type = typename std::common_type<
			typename T::type, typename Turn::type>::type;
	};

	static_assert(is_number<typename Turn::type>::value,
		"angle<T>: T::type must be a number");

	//befriend all angles
	template<class T> friend class angle;

public:
	//---------------------------------------------------------------
	// TYPES
	//---------------------------------------------------------------
	using turn_type   = Turn;
	using numeric_type = typename Turn::type;


	//---------------------------------------------------------------
	// CONSTANTS
	//---------------------------------------------------------------
	static constexpr numeric_type
	turn() noexcept {
		return (turn_type::value);
	}


	//---------------------------------------------------------------
	// CONSTRUCTION / DESTRUCTION
	//---------------------------------------------------------------
	///@brief no default contructor, angle has to be initialized with a value
	angle() = delete;

	//-----------------------------------------------------
	/// @brief implicit conversion from simple values is not allowed
	explicit constexpr
	angle(const numeric_type& a):
		v_{a}
	{}
	//-----------------------------------------------------
	/// @brief implicit conversion from simple values is not allowed
	template<class T, class = typename std::enable_if<is_number<T>::value>::type>
	explicit constexpr
	angle(const T& a):
		v_{numeric_type(a)}
	{
		AM_CHECK_NARROWING(numeric_type, T)
	}
	//-----------------------------------------------------
	/// @brief   implicit conversion of angles is allowed
	/// @details may cause numerical drift when performed repeatedly
	template<class T>
	constexpr
	angle(const angle<T>& a):
		v_{a.as<turn_type>()}
	{
		AM_CHECK_NARROWING(numeric_type, typename T::type)
	}

	//-----------------------------------------------------
	constexpr
	angle(const angle&) = default;

	//---------------------------------------------------------------
	template<class T>
	angle&
	operator = (const angle<T>& a)
	{
		AM_CHECK_NARROWING(numeric_type, typename T::type)

		v_ = a.as<turn_type>();
		return *this;
	}


	//---------------------------------------------------------------
	/// @brief maps the stored value to the range [0, turn]
	angle&
	wrap() {
		using std::fmod;
		if(v_ < numeric_type(0)) {
			v_ = -v_;
		}
		if(v_ > turn()) {
			v_ = fmod(v_, turn());
		}
		return *this;
	}


	//---------------------------------------------------------------
	// ASSIGNING ARITHMETIC OPERATIONS
	//---------------------------------------------------------------
	template<class T>
	angle&
	operator += (const angle<T>& a)
	{
		AM_CHECK_NARROWING(numeric_type, typename T::type)

		v_ += a.as<turn_type>();
		return *this;
	}
	//-----------------------------------------------------
	template<class T>
	angle&
	operator -= (const angle<T>& a)
	{
		AM_CHECK_NARROWING(numeric_type, typename T::type)

		v_ += a.as<turn_type>();
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<is_number<T>::value>::type>
	angle&
	operator *= (const T& factor)
	{
		AM_CHECK_NARROWING(numeric_type, typename T::type)

		v_ *= factor;
		return *this;
	}
	//-----------------------------------------------------
	template<class T, class = typename std::enable_if<is_number<T>::value>::type>
	angle&
	operator /= (const T& factor)
	{
		AM_CHECK_NARROWING(numeric_type, typename T::type)

		v_ /= factor;
		return *this;
	}


	//---------------------------------------------------------------
	// ARITHMETIC OPERATIONS
	//---------------------------------------------------------------
	template<class T>
	constexpr angle
	operator + (const angle<T>& a) const
	{
		return angle{v_ + a.as<turn_type>()};
	}
	//-----------------------------------------------------
	template<class T>
	constexpr angle
	operator - (const angle<T>& a) const
	{
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
	// increment/decrement operations
	//---------------------------------------------------------------
	angle
	operator ++ () {
		++v_;
		return *this;
	}
	//-----------------------------------------------------
	angle
	operator ++ (int) {
		auto old = angle{*this};
		operator++();
		return old;
	}
	//-----------------------------------------------------
	angle
	operator -- () {
		--v_;
		return *this;
	}
	//-----------------------------------------------------
	angle
	operator -- (int) {
		auto old = angle{*this};
		operator--();
		return old;
	}


	//---------------------------------------------------------------
	// COMPARISON
	//---------------------------------------------------------------
	template<class T>
	constexpr bool
	operator == (const angle<T>& other) {
		return (v_ == other.as<turn_type>());
	}
	//-----------------------------------------------------
	template<class T>
	constexpr bool
	operator != (const angle<T>& other) {
		return (v_ != other.as<turn_type>());
	}
	//-----------------------------------------------------
	template<class T>
	constexpr bool
	operator < (const angle<T>& other) {
		return (v_ < other.as<turn_type>());
	}
	//-----------------------------------------------------
	template<class T>
	constexpr bool
	operator > (const angle<T>& other) {
		return (v_ > other.as<turn_type>());
	}
	//-----------------------------------------------------
	template<class T>
	constexpr bool
	operator <= (const angle<T>& other) {
		return (v_ <= other.as<turn_type>());
	}
	//-----------------------------------------------------
	template<class T>
	constexpr bool
	operator >= (const angle<T>& other) {
		return (v_ >= other.as<turn_type>());
	}


	//---------------------------------------------------------------
	template<class OutTurn>
	inline friend constexpr typename conversion<OutTurn>::type
	angle_cast(const angle& a)	{
		return (a.as<OutTurn>());
	}

	//-----------------------------------------------------
	template<class T>
	inline friend constexpr T
	radians_cast(const angle& a) {
		return a.as<radians_turn<T>>();
	}

	//-----------------------------------------------------
	template<class T>
	inline friend constexpr T
	degrees_cast(const angle& a) {
		return a.as<degrees_turn<T>>();
	}
	//-----------------------------------------------------
	template<class T>
	inline friend constexpr T
	arcmins_cast(const angle& a) {
		return a.as<arcmins_turn<T>>();
	}
	//-----------------------------------------------------
	template<class T>
	inline friend constexpr T
	arcsecs_cast(const angle& a) {
		return a.as<arcsecs_turn<T>>();
	}

	//-----------------------------------------------------
	template<class T>
	inline friend constexpr T
	gons_cast(const angle& a) {
		return a.as<gons_turn<T>>();
	}
	//-----------------------------------------------------
	template<class T>
	inline friend constexpr T
	gon_cs_cast(const angle& a) {
		return a.as<gon_cs_turn<T>>();
	}
	//-----------------------------------------------------
	template<class T>
	inline friend constexpr T
	gon_ccs_cast(const angle& a) {
		return a.as<gon_ccs_turn<T>>();
	}


private:
	//---------------------------------------------------------------
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
		return ( (res_t(OutTurn::value) / res_t(turn())) * v_);
	}

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
using radians = angle<radians_turn<V>>;

//---------------------------------------------------------
template<class V>
using degrees = angle<degrees_turn<V>>;

template<class V>
using arcmins = angle<arcmins_turn<V>>;

template<class V>
using arcsecs = angle<arcsecs_turn<V>>;

//---------------------------------------------------------
template<class V>
using gons = angle<gons_turn<V>>;

template<class V>
using gon_cs = angle<gon_cs_turn<V>>;

template<class V>
using gon_ccs = angle<gon_ccs_turn<V>>;


//-------------------------------------------------------------------
using rad  = angle<radians_turn<real_t>>;
using radf = angle<radians_turn<float>>;
using radd = angle<radians_turn<double>>;

using deg  = angle<degrees_turn<real_t>>;
using degf = angle<degrees_turn<float>>;
using degd = angle<degrees_turn<double>>;
using degi = angle<degrees_turn<int>>;

using gon  = angle<gons_turn<real_t>>;
using gonf = angle<gons_turn<float>>;
using gond = angle<gons_turn<double>>;
using goni = angle<gons_turn<int>>;






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
constexpr arcmins<real_t> operator"" _arcmin (long double x) {
    return arcmins<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr arcmins<real_t> operator"" _arcmin (unsigned long long int x) {
    return arcmins<real_t>{real_t(x)};
}


//-------------------------------------------------------------------
constexpr arcsecs<real_t> operator"" _arcsec (long double x) {
    return arcsecs<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr arcsecs<real_t> operator"" _arcsec (unsigned long long int x) {
    return arcsecs<real_t>{real_t(x)};
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


//-------------------------------------------------------------------
constexpr gons<real_t> operator"" _goncs (long double x) {
    return gons<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr gons<real_t> operator"" _goncs (unsigned long long int x) {
    return gons<real_t>{real_t(x)};
}


//-------------------------------------------------------------------
constexpr gon_cs<real_t> operator"" _gonccs (long double x) {
    return gon_cs<real_t>{real_t(x)};
}
//---------------------------------------------------------
constexpr gon_ccs<real_t> operator"" _gonccs (unsigned long long int x) {
    return gon_ccs<real_t>{real_t(x)};
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
template<class Ostream, class T>
inline Ostream&
operator << (Ostream& os, const angle<T>& r)
{
	return (os << angle_cast<T>(r));
}


//-------------------------------------------------------------------
// pretty printing
//-------------------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<radians_turn<T>>& a)
{
	return (os << a << "rad");
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<degrees_turn<T>>& a)
{
	return (os << a << "°");
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<arcmins_turn<T>>& a)
{
	return (os << a << "'");
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<arcsecs_turn<T>>& a)
{
	return (os << a << "''");
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<gons_turn<T>>& a)
{
	return (os << a << "gon");
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<gon_cs_turn<T>>& a)
{
	return (os << a << "cs");
}

//---------------------------------------------------------
template<class T, class Ostream>
inline Ostream&
print(Ostream& os, const angle<gon_ccs_turn<T>>& a)
{
	return (os << a << "ccs");
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
struct is_angle :
	public std::integral_constant<bool,
		is_angle<T>::value && is_angle<R...>::value>
{};

//---------------------------------------------------------
template<class T>
struct is_angle<T> :
	public std::false_type
{};

//-----------------------------------------------------
template<class T>
struct is_angle<angle<T>> :
	public std::true_type
{};






/*****************************************************************************
 *
 *
 *
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
/// @brief maps the input angle to the turn range
template<class T>
inline constexpr angle<T>
wrapped(angle<T> a)
{
	return angle<T>{a.wrap()};
}


//-------------------------------------------------------------------
/// @brief returns turn - wrapped(angle)
template<class T>
inline angle<T>
turn_remainder(angle<T> a)
{
	auto r = angle<T>{a.turn() - angle_cast<T>(a)};
	r.wrap();
	return r;
}







/*****************************************************************************
 *
 *
 * FACTORIES
 *
 *
 *****************************************************************************/

//-------------------------------------------------------------------
template<class T>
inline constexpr radians<T>
make_radians(const T& t)
{
	return radians<T>{t};
}
//---------------------------------------------------------
template<class U>
inline constexpr radians<typename angle<U>::numeric_type>
make_radians(const angle<U>& a)
{
	return radians<typename angle<U>::numeric_type>{a};
}

//---------------------------------------------------------
template<class T>
inline constexpr degrees<T>
make_degrees(const T& t)
{
	return degrees<T>{t};
}
//---------------------------------------------------------
template<class U>
inline constexpr degrees<typename angle<U>::numeric_type>
make_degrees(const angle<U>& a)
{
	return degrees<typename angle<U>::numeric_type>{a};
}

//---------------------------------------------------------
template<class T>
inline constexpr gons<T>
make_gons(const T& t)
{
	return gons<T>{t};
}
//---------------------------------------------------------
template<class U>
inline constexpr gons<typename angle<U>::numeric_type>
make_gons(const angle<U>& a)
{
	return gons<typename angle<U>::numeric_type>{a};
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
template<class T>
inline typename std::common_type<typename T::type,real_t>::type
sin(const angle<T>& r)
{
	using std::sin;
	using res_t = typename std::common_type<typename T::type,real_t>::type;

	return sin(radians_cast<res_t>(r));
}

//-----------------------------------------------------
template<class T>
inline typename std::common_type<typename T::type,real_t>::type
cos(const angle<T>& r)
{
	using std::cos;
	using res_t = typename std::common_type<typename T::type,real_t>::type;

	return cos(radians_cast<res_t>(r));
}

//-----------------------------------------------------
template<class T>
inline typename std::common_type<typename T::type,real_t>::type
tan(const angle<T>& r)
{
	using std::tan;
	using res_t = typename std::common_type<typename T::type,real_t>::type;

	return tan(radians_cast<res_t>(r));
}



//-------------------------------------------------------------------
// HYPERBOLIC FUNCTIONS
//-------------------------------------------------------------------
template<class T>
inline typename std::common_type<typename T::type,real_t>::type
sinh(const angle<T>& r)
{
	using std::sinh;
	using res_t = typename std::common_type<typename T::type,real_t>::type;

	return sinh(radians_cast<res_t>(r));
}

//-----------------------------------------------------
template<class T>
inline typename std::common_type<typename T::type,real_t>::type
cosh(const angle<T>& r)
{
	using std::cosh;
	using res_t = typename std::common_type<typename T::type,real_t>::type;

	return cosh(radians_cast<res_t>(r));
}

//-----------------------------------------------------
template<class T>
inline typename std::common_type<typename T::type,real_t>::type
tanh(const angle<T>& r)
{
	using std::tanh;
	using res_t = typename std::common_type<typename T::type,real_t>::type;

	return tanh(radians_cast<res_t>(r));
}



//-------------------------------------------------------------------
// INVERSE TRIGONOMETRIC FUNCTIONS
//-------------------------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
rad_asin(T v)
{
	using std::asin;
	using res_t = typename std::common_type<T,real_t>::type;

	return radians<res_t>{res_t(asin(v))};
}

//-----------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
rad_acos(T v)
{
	using std::acos;
	using res_t = typename std::common_type<T,real_t>::type;

	return radians<res_t>{res_t(acos(v))};
}

//-----------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
rad_atan(T v)
{
	using std::atan;
	using res_t = typename std::common_type<T,real_t>::type;

	return radians<res_t>{res_t(atan(v))};
}

//-----------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
rad_atan2(T x, T y)
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
rad_asinh(T v)
{
	using std::asinh;
	using res_t = typename std::common_type<T,real_t>::type;

	return radians<res_t>{res_t(asinh(v))};
}

//-----------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
rad_acosh(T v)
{
	using std::acosh;
	using res_t = typename std::common_type<T,real_t>::type;

	return radians<res_t>{res_t(acosh(v))};
}

//-----------------------------------------------------
template<class T>
inline radians<typename std::common_type<T,real_t>::type>
rad_atanh(T v)
{
	using std::atanh;
	using res_t = typename std::common_type<T,real_t>::type;

	return radians<res_t>{res_t(atanh(v))};
}


}  // namespace num




#endif
